USE online_kursevi;
delimiter $$

DROP TRIGGER IF EXISTS triger1 $$
CREATE TRIGGER triger1 AFTER INSERT ON ciklus
FOR EACH ROW
BEGIN
	DECLARE profesor VARCHAR(45);
	
	IF(NEW.datum_pocetka > NEW.datum_kraja) THEN
		SIGNAL SQLSTATE '45000' SET message_text = 'Datum pocetka mora biti manji od datuma kraja';
	END IF;
	
	-- Prednost imaju profesori koji nisu predavali
	SET profesor = (SELECT	Korisnik_kor_ime
					FROM	predavac p
					WHERE	NOT EXISTS(
						SELECT	*
						FROM	predaje pd
						WHERE	pd.Predavac_Korisnik_kor_ime = p.Korisnik_kor_ime
						)
					LIMIT 1);
					
	-- Ako su svi predavali bar jednom, odaberi onog ko je slobodan
	IF(profesor IS NULL) THEN
		SET profesor = (SELECT	Korisnik_kor_ime
					FROM	predavac p
					WHERE	NOT EXISTS(
						SELECT	*
						FROM	predaje pd
							JOIN	ciklus c
								ON	c.datum_pocetka = pd.Ciklus_datum_pocetka AND
									c.Kurs_sifra = pd.Ciklus_Kurs_sifra
						WHERE	p.Korisnik_kor_ime = pd.Predavac_Korisnik_kor_ime AND (
									(c.datum_pocetka >= NEW.datum_pocetka AND c.datum_pocetka < NEW.datum_kraja) OR
									(c.datum_kraja > NEW.datum_pocetka AND c.datum_kraja <= NEW.datum_kraja)
								)
						)
					LIMIT 1);
	END IF;
	
	IF(profesor IS NULL) THEN
		SIGNAL SQLSTATE '45000' SET message_text = 'Ne postoji nijedan dostupan profesor';
	ELSE
		INSERT INTO predaje(Ciklus_datum_pocetka, Ciklus_Kurs_sifra, Predavac_Korisnik_kor_ime) VALUES (NEW.datum_pocetka, NEW.Kurs_sifra, profesor);
	END IF;
END $$

/* nove unose je potrebno pravilno inicijalizovati (dodati sertifikate, izmeniti poene, itd) */
DROP TRIGGER IF EXISTS triger2_1 $$
CREATE TRIGGER triger2_1 AFTER INSERT ON prijavljuje
FOR EACH ROW
BEGIN
	DECLARE dat_kr DATE;
	DECLARE trajanje INTEGER;
	DECLARE max_ucesnika INTEGER;
	DECLARE br_ucesnika INTEGER;
	
	IF(NEW.progres > 100) THEN
		SIGNAL SQLSTATE '45000' SET message_text = 'Progres ne moze biti veci od 100';
	END IF;
	
	SET br_ucesnika = (SELECT COUNT(*) FROM prijavljuje WHERE Ciklus_datum_pocetka = NEW.Ciklus_datum_pocetka AND Ciklus_Kurs_sifra = NEW.Ciklus_Kurs_sifra);
	SELECT k.trajanje, k.max_ucesnika INTO trajanje, max_ucesnika FROM kurs k WHERE k.sifra = NEW.Ciklus_Kurs_sifra;
	
	IF(EXISTS(SELECT * FROM sertifikat WHERE Prijavljuje_Ucesnik_Korisnik_kor_ime = NEW.Ucesnik_Korisnik_kor_ime AND Prijavljuje_Ciklus_Kurs_sifra = NEW.Ciklus_Kurs_sifra)) THEN
		SIGNAL SQLSTATE '45000' SET message_text = 'Ucesnik je vec zavrsio ovaj kurs';
	END IF;
	
	IF(br_ucesnika > max_ucesnika) THEN
		SIGNAL SQLSTATE '45000' SET message_text = 'Dostignut je maksimalan broj ucesnika';
	END IF;
	
	IF(NEW.progres = 100) THEN
		SET dat_kr = (SELECT c.datum_kraja FROM ciklus c WHERE c.datum_pocetka = NEW.Ciklus_datum_pocetka AND c.Kurs_sifra = NEW.Ciklus_Kurs_sifra);
		INSERT INTO sertifikat(datum_dodele, Prijavljuje_Ciklus_datum_pocetka, Prijavljuje_Ciklus_Kurs_sifra, Prijavljuje_Ucesnik_Korisnik_kor_ime) VALUES (DATE_ADD(dat_kr, INTERVAL 1 WEEK), NEW.Ciklus_datum_pocetka, NEW.Ciklus_Kurs_sifra, NEW.Ucesnik_Korisnik_kor_ime);
	END IF;
	
	SET trajanje = (SELECT k.trajanje FROM kurs k WHERE k.sifra = NEW.Ciklus_Kurs_sifra);
	UPDATE ucesnik SET poeni = poeni + CEIL((trajanje * 1.0 / 100) * NEW.progres) WHERE Korisnik_kor_ime = NEW.Ucesnik_Korisnik_kor_ime;
END $$

/* ovaj triger se odnosi na azuriranje postojeceg reda u prijavama*/
DROP TRIGGER IF EXISTS triger2_2 $$
CREATE TRIGGER triger2_2 AFTER UPDATE ON prijavljuje
FOR EACH ROW
BEGIN
	DECLARE dat_kr DATE;
	DECLARE trajanje INTEGER;
	
	IF(NEW.progres > 100) THEN
		SIGNAL SQLSTATE '45000' SET message_text = 'Progres ne moze biti veci od 100';
	END IF;
	
	IF(NEW.progres - OLD.progres <= 0) THEN
		SIGNAL SQLSTATE '45000' SET message_text = 'Novi progres mora biti veci od prethodnog';
	END IF;
	
	IF(NEW.progres = 100) THEN
		SET dat_kr = (SELECT c.datum_kraja FROM ciklus c WHERE c.datum_pocetka = NEW.Ciklus_datum_pocetka AND c.Kurs_sifra = NEW.Ciklus_Kurs_sifra);
		INSERT INTO sertifikat(datum_dodele, Prijavljuje_Ciklus_datum_pocetka, Prijavljuje_Ciklus_Kurs_sifra, Prijavljuje_Ucesnik_Korisnik_kor_ime) VALUES (DATE_ADD(dat_kr, INTERVAL 1 WEEK), NEW.Ciklus_datum_pocetka, NEW.Ciklus_Kurs_sifra, NEW.Ucesnik_Korisnik_kor_ime);
	END IF;
	
	SET trajanje = (SELECT k.trajanje FROM kurs k WHERE k.sifra = NEW.Ciklus_Kurs_sifra);
	UPDATE ucesnik SET poeni = poeni + CEIL((trajanje * 1.0 / 100) * (NEW.progres - OLD.progres)) WHERE Korisnik_kor_ime = NEW.Ucesnik_Korisnik_kor_ime;
END $$