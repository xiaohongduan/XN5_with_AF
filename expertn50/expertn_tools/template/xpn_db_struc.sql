-- Table: BewegungBeregnung
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE BewegungBeregnung ( 
    AutoTeilschlag   INT,
    Beregnungstermin DATE,
    Beregnungsart    TEXT,
    Beregnungsmenge  INT 
);


-- Table: BewegungBodenbearbeitung
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE BewegungBodenbearbeitung ( 
    AutoTeilschlag     INT,
    Bearbeitungstermin DATE,
    Bearbeitungstiefe  REAL,
    Bearbeitungsgeraet TEXT 
);


-- Table: BewegungMeßwerteBoden
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE BewegungMeßwerteBoden ( 
    AutoTeilschlag      REAL,
    TerminMeßwerte      DATE,
    Schichtnummer       REAL,
    Schichtmaechtigkeit REAL,
    NitratgehaltBoden   REAL,
    AmmoniumgehaltBoden REAL,
    WassergehaltBoden   REAL,
    Matrixpotential     TEXT,
    Wasserflussdichte   TEXT 
);


-- Table: BewegungMeßwertePflanze
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE BewegungMeßwertePflanze ( 
    AutoTeilschlag                            INT,
    TerminMesswerte                           DATE,
    Entwicklungsstadium                       REAL,
    Pflanzenhoehe                             INT,
    Blattfaechenindex                         REAL,
    Blattanzahl                               INT,
    Pflanzenzahl                              INT,
    OberirdischeTrockenmasseBlaetter          INT,
    OberirdischeTrockenmasseHalmeStengel      INT,
    OberirdischeTrockenmasseFrucht            INT,
    OberirdischeTrockenmasseGesamt            INT,
    NKonzOberirdischeTrockenmasseBlaetter     REAL,
    NKonzOberirdischeTrockenmasseHalmeStengel REAL,
    NKonzOberirdischeTrockenmasseFrucht       REAL,
    NKonzOberirdischeTrockenmasseGesamt       REAL,
    Durchwurzelungstiefe                      INT,
    Wurzeltrockenmasse                        INT,
    NKonzWurzeltrockenmasse                   REAL,
    Pflanzenbedeckungsgrad                    REAL 
);


-- Table: BewegungMineraldüngung
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE BewegungMineraldüngung ( 
    AutoTeilschlag         INT,
    Düngerart              TEXT,
    Ausbringungstermin     DATE,
    Ausbringungsmenge      INT,
    GesamtNGehaltDuenger   REAL,
    NitratNGehaltDuenger   REAL,
    AmmoniumNGehaltDuenger REAL,
    AmidNGehalDuenger      REAL 
);


-- Table: BewegungOrganischeDüngung
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE BewegungOrganischeDüngung ( 
    AutoTeilschlag            INT,
    Duengerart                TEXT,
    Ausbringungstermin        DATE,
    Ausbringungsmenge         INT,
    MengeDimension            TEXT,
    TrockenSubstanzDuenger    REAL,
    OrganischeSubstanzDuenger REAL,
    GehaltGesamtNDuenger      REAL,
    NH4NDuenger               REAL 
);


-- Table: BewegungPflanzendaten
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE BewegungPflanzendaten ( 
    AutoTeilschlag          INT,
    Fruchtart               TEXT,
    Saatstaerke             INT,
    Reihenabstand           REAL,
    Saattiefe               REAL,
    Saattermin              DATE,
    MaxDurchwurzelungstiefe INT,
    TerminAuflaufen         DATE,
    TerminErnteNutzung      DATE,
    TerminMaxBioMasse       DATE,
    TerminMaxDruchwurzelung DATE,
    AbuhrBiomasse           INT,
    Sorte                   TEXT 
);


-- Table: BewegungStartwerte
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE BewegungStartwerte ( 
    AutoTeilschlag          INT,
    Termin                  DATE,
    Schichtnummer           INT,
    Schichtdicke            REAL,
    NitratgehaltBoden       REAL,
    AmmoniumgehaltBoden     REAL,
    WassergehaltBoden       REAL,
    WassergehaltDimension   TEXT,
    Bodenemperatur          REAL,
    Wurzeldichtenverteilung INT 
);


-- Table: BewegungVorfrucht
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE BewegungVorfrucht ( 
    AutoTeilschlag                         INT,
    Vorfruchterntetermin                   DATE,
    Vorfruchtart                           TEXT,
    Vorfruchtertrag                        REAL,
    AbfuhrVorfruchternterückstände         INT,
    MengeOberirdischeVorfruchtrueckstaende REAL,
    MengeVorfruchtwurzelrueckstaende       REAL,
    CnOberirdischeVorfruchtrueckstaende    REAL,
    CnVorfruchtwurzelrueckstaende          REAL 
);


-- Table: Einfügefehler
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE Einfügefehler ( 
    Profilname                        TEXT,
    [Mittlerer Grundwasserstand (cm)] REAL,
    Staunässestufe                    TEXT,
    [Dräenabstand (cm)]               REAL,
    [Dräntiefe (cm)]                  REAL,
    NutzungLdw                        INT 
);


-- Table: Projekt
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE Projekt ( 
    Projektname      TEXT,
    AutoTeilschlag   INT,
    DatumStart       DATE,
    DatumEnde        DATE,
    Betriebsnummer   REAL,
    Betriebsname     TEXT,
    Jahr             INT,
    Schlagnummer     TEXT,
    Schlagname       TEXT,
    Teilschlagnummer TEXT,
    Teilschlagname   TEXT,
    Fitting          INT,
    PRIMARY KEY ( Projektname, AutoTeilschlag ) 
);


-- Table: StammBetrieb
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE StammBetrieb ( 
    Betriebsnummer    REAL,
    Betriebsname      TEXT,
    Laengengrad       REAL,
    Breitengrad       REAL,
    NameBesitzer      TEXT,
    VornameBesitzer   TEXT,
    StrasseHausnummer TEXT,
    Postleitzahl      INT,
    Ort               TEXT,
    Vorwahl           TEXT,
    Telefon           TEXT,
    Fax               TEXT,
    PRIMARY KEY ( Betriebsnummer ) 
);


-- Table: StammBodenprofilGesamt
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE StammBodenprofilGesamt ( 
    NameBodenprofil           TEXT,
    MittlererGrundwasserstand REAL,
    Staunässestufe            TEXT,
    Draenabstand              REAL,
    Draentiefe                REAL,
    NutzungLdw                INT,
    PRIMARY KEY ( NameBodenprofil ) 
);


-- Table: StammBodenprofilSchichten
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE StammBodenprofilSchichten ( 
    NameBodenprofil                TEXT,
    Schichtnummer                  INT,
    Schichtmacht                   REAL,
    AnzSimSchichten                INT,
    Lagerungsdichte                REAL,
    Bodenart                       TEXT,
    Tongehalt                      REAL,
    Schluffgehalt                  REAL,
    Sandgehalt                     REAL,
    Steingehalt                    REAL,
    GehaltOrganischeSubstanz       REAL,
    GehaltOrganischerKohlenstoff   REAL,
    GehaltOrganischerStickstoff    REAL,
    Ph                             REAL,
    Austauschkapazitaet            REAL,
    Gesamtporenvolumen             REAL,
    Feldkapazitaet                 REAL,
    PermanenterWelkepunkt          REAL,
    MaximalesPotential             REAL,
    GesaettigteLeitfaehigkeit      REAL,
    ModellgroesseWendepunktVol     REAL,
    ModellgroesseWendepunkPa       REAL,
    ModellgroesseVanGenuchtenAlpha REAL,
    ModellgroesseVanGenuchtenN     REAL,
    ModellgroesseAev               REAL,
    ModellgroesseCampellB          REAL,
    ModellgroesseWasserRes         REAL,
    PRIMARY KEY ( NameBodenprofil, Schichtnummer ) 
);


-- Table: StammJahr
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE StammJahr ( 
    Betriebsnummer REAL,
    AutoJahr       INTEGER PRIMARY KEY AUTOINCREMENT,
    Jahr           INT 
);


-- Table: StammSchlag
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE StammSchlag ( 
    AutoJahr      INT,
    AutoSchlag    INTEGER PRIMARY KEY AUTOINCREMENT,
    Schlagnummer  TEXT,
    Schlagname    TEXT,
    Schlaggroesse REAL 
);


-- Table: StammTeilschlag
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE StammTeilschlag ( 
    AutoSchlag               INT,
    AutoTeilschlag           INTEGER PRIMARY KEY AUTOINCREMENT,
    Teilschlagnummer         TEXT,
    Teilschlagname           TEXT,
    Teilschlaggroesse        REAL,
    NameBodenprofil          TEXT,
    NameWetterStation        TEXT,
    NameKonfigurationsDatei  TEXT,
    NameModellparameterDatei TEXT,
    Hoehelage                INT,
    Hangneigung              REAL,
    Exposition               TEXT,
    Gebietsnummer            TEXT 
);


-- Table: StammWetterStation
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE StammWetterStation ( 
    NameWetterstation                    TEXT,
    BreitengradWetterstation             REAL,
    LaengengradWetterstation             REAL,
    HoehenlageWetterstation              REAL,
    HoeheTemperaturmessung               REAL,
    HoeheWindmessung                     REAL,
    Jahresdurchschnittstemperatur        REAL,
    DurchschnAmplitudeLufttemperaturJahr REAL,
    PRIMARY KEY ( NameWetterstation ) 
);


-- Table: StammWetterTageswerte
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE StammWetterTageswerte ( 
    NameWetterstation             TEXT,
    DatumTageswerte               DATE,
    Globalstrahlung               REAL,
    MaximaleLufttemperatur        REAL,
    MinimaleLufttemperatur        REAL,
    Niederschlag                  REAL,
    Sonnenscheindauer             REAL,
    RelativeLuftfeuchte           REAL,
    Windgeschwindigkeit           REAL,
    MittlereLufttemperatur        REAL,
    Taupunkt                      REAL,
    Kesselverdunstung             REAL,
    SaettigungsdefizitLuft        REAL,
    Schneehoehe                   REAL,
    PhotosynthischAktiveStrahlung REAL,
    Bodentemperatur2Cm            REAL,
    Bodentemperatur5Cm            REAL,
    Bodentemperatur10Cm           REAL,
    Bodentemperatur20Cm           REAL,
    Bodentemperatur50Cm           REAL,
    Bestandesniederschlag         INT,
    Grundwasserspiegel            INT,
    PRIMARY KEY ( NameWetterstation, DatumTageswerte ) 
);


-- Table: TabelleBeregnungsart
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleBeregnungsart ( 
    Bergnungsart TEXT,
    Code         TEXT,
    PRIMARY KEY ( Bergnungsart ) 
);


-- Table: TabelleBodenarten
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleBodenarten ( 
    ADV                        TEXT,
    SchluesselKartieranleitung TEXT,
    tBereichTon                TEXT,
    tBereichSchluff            TEXT,
    tBereichSand               TEXT,
    sGehaltTon                 REAL,
    sGehaltSchluff             REAL,
    sGehaltSand                REAL,
    Code                       TEXT,
    Bodenart                   TEXT,
    [nfk(Ld:1-2)]              REAL,
    [nfk(Ld:3)]                REAL,
    [nfk(Ld:4-5)]              REAL,
    [FK(Ld:1-2)]               REAL,
    [FK(Ld:3)]                 REAL,
    [FK(Ld:4-5)]               REAL,
    [GPV(Ld:1-2)]              REAL,
    [GPV(Ld:3)]                REAL,
    [GPV(Ld:4-5)]              REAL,
    [PWP(Ld:1-2)]              REAL,
    [PWP(Ld:3)]                REAL,
    [PWP(Ld:4-5)]              REAL 
);

INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ls', 'sandiger Lehm', '15-25', '15-50', '25-68', '20.0', '32.0', '48.0', 'SALO', 'Ls', '22.0', '17.0', '14.0', '38.0', '33.0', '31.0', '52.0', '43.0', '37.0', '16.0', '16.0', '17.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Lt', 'toniger Lehm', '25-45', '18-70', '0-57', '35.0', '44.0', '21.0', 'CLLO', 'Lt', '20.0', '16.0', '12.0', '46.0', '41.0', '37.0', '57.0', '48.0', '41.0', '26.0', '25.0', '25.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('S', 'Sand', ' 0-5', ' 0-10', ' 85-100', '2.5', '5.0', '92.5', 'SA', 'S', '10.0', '9.0', '9.0', '14.0', '12.0', '12.0', '41.0', '36.0', '31.0', '4.0', '3.0', '3.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Sl', 'lehmiger Sand', '5-17', '5-50', '35-90', '11.0', '22.0', '77.0', 'LOSA', 'Sl', '22.0', '18.0', '16.0', '32.0', '29.0', '24.0', '51.0', '42.0', '34.0', '10.0', '11.0', '8.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('St', 'toniger Sand', '5-25', '0-15', '60-95', '15.0', '7.0', '78.0', 'CLSA', 'St', '19.0', '15.0', '13.0', '30.0', '24.0', '21.0', '48.0', '40.0', '32.0', '11.0', '9.0', '8.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Su', 'schluffiger Sand', '0-8', '10-50', '42-90', '4.0', '30.0', '66.0', 'SISA', 'Su', '21.0', '18.0', '17.0', '31.0', '24.0', '24.0', '50.0', '41.0', '33.0', '10.0', '6.0', '7.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ts', 'sandiger Ton', '25-65', '0-18', '17-75', '45.0', '9.0', '46.0', 'SACL', 'Ts', '22.0', '16.0', '12.0', '47.0', '41.0', '37.0', '58.0', '48.0', '41.0', '25.0', '25.0', '25.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ts2', 'schwach sandiger Ton', ' 50-65', ' 0-18', ' 17-50', '57.5', '9.0', '33.5', 'LSACL', 'Ts2', '21.0', '17.0', '12.0', '47.0', '42.0', '38.0', '57.0', '49.0', '42.0', '26.0', '25.0', '26.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ts3', 'mittel sandiger Ton', ' 35-50', ' 0-18', ' 32-65', '42.5', '9.0', '48.5', 'MSAC', 'Ts3', '21.0', '17.0', '12.0', '47.0', '42.0', '38.0', '57.0', '49.0', '42.0', '26.0', '25.0', '26.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ts4', 'stark sandiger Ton', ' 25-35', ' 0-18', ' 47-75', '30.0', '9.0', '61.0', 'VSACL', 'Ts4', '21.0', '17.0', '12.0', '47.0', '42.0', '38.0', '57.0', '49.0', '42.0', '26.0', '25.0', '26.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Tu', 'schluffiger Ton', '25-65', '30-75', '0-30', '45.0', '52.0', '3.0', 'SICLL', 'Tu', '19.0', '15.0', '12.0', '46.0', '41.0', '36.0', '56.0', '48.0', '40.0', '27.0', '26.0', '24.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ul', 'lehmiger Schluff', '8-30', '50-92', '0-42', '19.0', '71.0', '10.0', 'LOSI', 'Ul', '26.0', '23.0', '20.0', '38.0', '34.0', '33.0', '50.0', '44.0', '38.0', '12.0', '11.0', '13.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ut', 'toniger Schluff', '8-25', '50-92', '0-42', '17.0', '71.0', '12.0', 'CASI', 'Ut', '26.0', '23.0', '20.0', '38.0', '34.0', '33.0', '50.0', '44.0', '38.0', '12.0', '11.0', '13.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ls2', 'schwach sandiger Lehm', ' 15-25', ' 40-50', ' 25-45', '20.0', '45.0', '35.0', 'LSALO', 'Ls2', '23.0', '17.0', '14.0', '38.0', '33.0', '31.0', '52.0', '43.0', '36.0', '15.0', '16.0', '17.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ls3', 'mittel sandiger Lehm', ' 15-25', ' 28-40', ' 35-55', '20.0', '35.0', '45.0', 'MSALO', 'Ls3', '22.0', '17.0', '14.0', '38.0', '33.0', '31.0', '52.0', '43.0', '37.0', '16.0', '16.0', '17.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ls4', 'stark sandiger Lehm', ' 17-25', ' 15-28', ' 47-68', '21.0', '21.5', '57.5', 'VSALO', 'Ls4', '22.0', '17.0', '14.0', '37.0', '32.0', '30.0', '52.0', '43.0', '37.0', '15.0', '15.0', '16.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Lt2', 'schwach toniger Lehm', ' 25-35', ' 35-50', ' 15-40', '30.0', '42.5', '27.5', 'LCLLO', 'Lt2', '19.0', '15.0', '12.0', '46.0', '41.0', '36.0', '56.0', '48.0', '40.0', '27.0', '26.0', '24.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Lt3', 'mittel toniger Lehm', ' 35-45', ' 30-50', ' 5-35', '40.0', '40.0', '20.0', 'MCA-', 'Lt3', '19.0', '15.0', '12.0', '46.0', '41.0', '36.0', '56.0', '48.0', '40.0', '27.0', '26.0', '24.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Lts', 'sandig-toniger Lehm', ' 25-45', ' 18-35', ' 25-57', '34.0', '25.0', '41.0', 'SACLL', 'Lts', '22.0', '16.0', '12.0', '47.0', '41.0', '37.0', '58.0', '48.0', '41.0', '25.0', '25.0', '25.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ltu', 'schluffig-toniger Lehm', ' 30-45', ' 50-70', ' 0-20', '34.0', '58.0', '8.0', 'SICLL', 'Ltu', '21.0', '17.0', '12.0', '47.0', '42.0', '38.0', '57.0', '49.0', '42.0', '26.0', '25.0', '26.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Lu', 'schluffiger Lehm', ' 17-30', ' 50-70', ' 0-33', '23.5', '60.0', '16.5', 'SILO', 'Lu', '24.0', '19.0', '16.0', '40.0', '36.0', '33.0', '52.0', '45.0', '38.0', '16.0', '17.0', '17.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Sl2', 'schwach lehmiger Sand', ' 5-8', ' 5-25', ' 67-90', '6.5', '15.0', '78.5', 'LLOSA', 'Sl2', '20.0', '16.0', '14.0', '27.0', '22.0', '21.0', '50.0', '41.0', '32.0', '7.0', '6.0', '7.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Sl3', 'mittel lehmiger Sand', ' 8-12', ' 7-40', ' 48-85', '10.0', '23.5', '66.5', 'MLOSA', 'Sl3', '23.0', '17.0', '15.0', '32.0', '27.0', '26.0', '51.0', '42.0', '35.0', '9.0', '10.0', '11.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Sl4', 'stark lehmiger Sand', ' 12-17', ' 13-40', ' 45-75', '14.5', '26.5', '59.0', 'VLOSA', 'Sl4', '23.0', '16.0', '14.0', '34.0', '28.0', '27.0', '52.0', '42.0', '35.0', '11.0', '12.0', '13.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Slu', 'schluffig-lehmiger Sand', ' 8-15', ' 40-50', ' 35-52', '11.5', '45.0', '43.5', 'SILOSA', 'Slu', '24.0', '19.0', '17.0', '34.0', '30.0', '27.0', '49.0', '42.0', '35.0', '10.0', '11.0', '10.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('St2', 'schwach toniger Sand', ' 5-15', ' 0-13', ' 72-95', '10.0', '6.5', '83.5', 'LCLSA', 'St2', '18.0', '14.0', '13.0', '29.0', '22.0', '20.0', '48.0', '40.0', '33.0', '11.0', '8.0', '7.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('St3', 'mittel toniger Sand', ' 15-25', ' 0-15', ' 60-85', '20.0', '7.5', '72.5', 'MCLSA', 'St3', '20.0', '15.0', '13.0', '32.0', '27.0', '22.0', '48.0', '40.0', '30.0', '12.0', '12.0', '9.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Su2', 'schwach schluffiger Sand', ' 0-5', ' 10-25', ' 70-90', '2.5', '17.5', '80.0', 'LSISA', 'Su2', '21.0', '18.0', '17.0', '31.0', '24.0', '24.0', '50.0', '41.0', '33.0', '10.0', '6.0', '7.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Su3', 'mittel schluffiger Sand', ' 0-8', ' 25-40', '52-75', '4.0', '32.5', '63.5', 'MSISA', 'Su3', '21.0', '18.0', '17.0', '31.0', '24.0', '24.0', '50.0', '41.0', '33.0', '10.0', '6.0', '7.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Su4', 'stark schluffiger Sand', ' 0-8', ' 40-50', ' 42-60', '4.0', '45.0', '51.0', 'VSISA', 'Su4', '21.0', '18.0', '17.0', '31.0', '24.0', '24.0', '50.0', '41.0', '33.0', '10.0', '6.0', '7.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('T', 'Ton', ' 65-100', ' 0-35', ' 0-35', '77.5', '12.5', '10.0', 'CLOSA', 'T', '20.0', '15.0', '11.0', '59.0', '54.0', '49.0', '66.0', '58.0', '50.0', '39.0', '39.0', '38.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Tl', 'lehmiger Ton', ' 45-65', ' 18-55', ' 0-37', '50.0', '34.0', '16.0', 'LOCL', 'Tl', '20.0', '14.0', '11.0', '55.0', '49.0', '45.0', '63.0', '53.0', '47.0', '35.0', '35.0', '34.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Tu2', 'schwach schluffiger Ton', ' 45-65', ' 30-55', ' 0-25', '52.0', '39.0', '9.0', 'LSICL', 'Tu2', '20.0', '14.0', '11.0', '55.0', '49.0', '45.0', '63.0', '53.0', '47.0', '35.0', '35.0', '34.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Tu3', 'mittel schluffiger Ton', ' 35-45', ' 40-65', ' 0-25', '40.0', '50.0', '10.0', 'MSICL', 'Tu3', '19.0', '15.0', '12.0', '46.0', '41.0', '36.0', '56.0', '48.0', '40.0', '27.0', '26.0', '24.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Tu4', 'stark schluffiger Ton', ' 25-35', ' 45-75', ' 0-30', '27.0', '58.0', '15.0', 'VSICL', 'Tu4', '21.0', '17.0', '12.0', '47.0', '42.0', '38.0', '57.0', '49.0', '42.0', '26.0', '25.0', '26.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('U', 'Schluff', ' 0-8', ' 80-100', ' 0-20', '3.0', '89.0', '8.0', 'SICLL', 'U', '28.0', '25.0', '23.0', '37.0', '34.0', '31.0', '51.0', '44.0', '37.0', '9.0', '9.0', '8.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ul2', 'schwach lehmiger Schluff', ' 8-12', ' 65-92', ' 0-27', '9.0', '78.5', '12.5', 'LLOSI', 'Ul2', '27.0', '25.0', '21.0', '38.0', '36.0', '32.0', '43.0', '44.0', '37.0', '11.0', '11.0', '11.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ul3', 'mittel lehmiger Schluff', ' 12-17', ' 65-88', ' 0-23', '13.0', '76.5', '10.5', 'MLOSI', 'Ul3', '27.0', '24.0', '20.0', '40.0', '37.0', '34.0', '53.0', '44.0', '39.0', '13.0', '13.0', '14.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ul4', 'stark lehmiger Schluff', ' 17-30', ' 70-83', ' 0-13', '21.0', '74.5', '4.5', 'VLOSI', 'Ul4', '26.0', '21.0', '19.0', '40.0', '37.0', '35.0', '53.0', '45.0', '40.0', '14.0', '16.0', '16.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Uls', 'sandig-lehmiger Schluff', ' 8-17', ' 50-65', ' 18-42', '12.5', '57.5', '30.0', '??????', 'Uls', '26.0', '22.0', '20.0', '37.0', '33.0', '30.0', '50.0', '42.0', '35.0', '11.0', '11.0', '10.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Us', 'sandiger Schluff', ' 0-8', ' 50-80', ' 12-50', '4.0', '65.0', '31.0', 'SASI', 'Us', '26.0', '22.0', '19.0', '35.0', '33.0', '29.0', '50.0', '44.0', '36.0', '9.0', '11.0', '10.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ut2', 'schwach toniger Schluff', ' 8-12', ' 50-92', ' 0-42', '9.0', '70.0', '21.0', 'LCASI', 'Ut2', '27.0', '25.0', '21.0', '38.0', '36.0', '32.0', '43.0', '44.0', '37.0', '11.0', '11.0', '11.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ut3', 'mittel toniger Schluff', ' 12-17', ' 50-88', ' 0-38', '13.0', '68.0', '19.0', 'MCLSI', 'Ut3', '27.0', '24.0', '20.0', '40.0', '37.0', '34.0', '53.0', '44.0', '39.0', '13.0', '13.0', '14.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('Ut4', 'stark toniger Schluff', ' 17-25', ' 50-83', ' 0-33', '20.0', '65.0', '15.0', 'VCLSI', 'Ut4', '26.0', '21.0', '19.0', '40.0', '37.0', '35.0', '53.0', '45.0', '40.0', '14.0', '16.0', '16.0');
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('OM', 'organic materials', 0, 0, 0, '0.0', '0.0', '0.0', 'OM', 'OM', null, null, null, null, null, null, null, null, null, null, null, null);
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('W', 'water', 0, 0, 0, '0.0', '0.0', '0.0', 'W', 'W', null, null, null, null, null, null, null, null, null, null, null, null);
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('BR', 'bedrock', 0, 0, 0, '0.0', '0.0', '0.0', 'BR', 'BR', null, null, null, null, null, null, null, null, null, null, null, null);
INSERT INTO [TabelleBodenarten] ([ADV], [SchluesselKartieranleitung], [tBereichTon], [tBereichSchluff], [tBereichSand], [sGehaltTon], [sGehaltSchluff], [sGehaltSand], [Code], [Bodenart], [nfk(Ld:1-2)], [nfk(Ld:3)], [nfk(Ld:4-5)], [FK(Ld:1-2)], [FK(Ld:3)], [FK(Ld:4-5)], [GPV(Ld:1-2)], [GPV(Ld:3)], [GPV(Ld:4-5)], [PWP(Ld:1-2)], [PWP(Ld:3)], [PWP(Ld:4-5)]) VALUES ('O', 'other', 0, 0, 0, '0.0', '0.0', '0.0', 'O', 'O', null, null, null, null, null, null, null, null, null, null, null, null);



-- Table: TabelleBodenOSForst
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleBodenOSForst ( 
    OrganischeSubstanzBereich TEXT,
    OrganischeSubstanzWert    REAL,
    Bezeichnung               TEXT 
);

INSERT INTO [TabelleBodenOSForst] ([OrganischeSubstanzBereich], [OrganischeSubstanzWert], [Bezeichnung]) VALUES ('< 1', '0.5', 'sehr schwach humos');
INSERT INTO [TabelleBodenOSForst] ([OrganischeSubstanzBereich], [OrganischeSubstanzWert], [Bezeichnung]) VALUES ('1 - 2', '1.5', 'schwach humos');
INSERT INTO [TabelleBodenOSForst] ([OrganischeSubstanzBereich], [OrganischeSubstanzWert], [Bezeichnung]) VALUES ('2 - 5', '3.5', '(mittel) humos');
INSERT INTO [TabelleBodenOSForst] ([OrganischeSubstanzBereich], [OrganischeSubstanzWert], [Bezeichnung]) VALUES ('5 - 10', '7.5', 'stark humos');
INSERT INTO [TabelleBodenOSForst] ([OrganischeSubstanzBereich], [OrganischeSubstanzWert], [Bezeichnung]) VALUES ('10 - 15', '12.5', 'sehr stark humos');
INSERT INTO [TabelleBodenOSForst] ([OrganischeSubstanzBereich], [OrganischeSubstanzWert], [Bezeichnung]) VALUES ('15 - 30', '22.5', 'extrem humos');
INSERT INTO [TabelleBodenOSForst] ([OrganischeSubstanzBereich], [OrganischeSubstanzWert], [Bezeichnung]) VALUES ('> 30', '35.0', 'Torf');

-- Table: TabelleBodenOSLandwirtschaft
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleBodenOSLandwirtschaft ( 
    OrganischeSubstanzBereich TEXT,
    OrganischeSubstanzWert    REAL,
    Bezeichnung               TEXT 
);

INSERT INTO [TabelleBodenOSLandwirtschaft] ([OrganischeSubstanzBereich], [OrganischeSubstanzWert], [Bezeichnung]) VALUES ('< 1', '0.5', 'sehr schwach humos');
INSERT INTO [TabelleBodenOSLandwirtschaft] ([OrganischeSubstanzBereich], [OrganischeSubstanzWert], [Bezeichnung]) VALUES ('1 - 2', '1.5', 'schwach humos');
INSERT INTO [TabelleBodenOSLandwirtschaft] ([OrganischeSubstanzBereich], [OrganischeSubstanzWert], [Bezeichnung]) VALUES ('2 - 4', '3.0', '(mittel) humos');
INSERT INTO [TabelleBodenOSLandwirtschaft] ([OrganischeSubstanzBereich], [OrganischeSubstanzWert], [Bezeichnung]) VALUES ('4 - 8', '6.0', 'stark humos');
INSERT INTO [TabelleBodenOSLandwirtschaft] ([OrganischeSubstanzBereich], [OrganischeSubstanzWert], [Bezeichnung]) VALUES ('8 - 15', '11.5', 'sehr stark humos');
INSERT INTO [TabelleBodenOSLandwirtschaft] ([OrganischeSubstanzBereich], [OrganischeSubstanzWert], [Bezeichnung]) VALUES ('15 - 30', '22.5', 'extrem humos');
INSERT INTO [TabelleBodenOSLandwirtschaft] ([OrganischeSubstanzBereich], [OrganischeSubstanzWert], [Bezeichnung]) VALUES ('> 30', '35.0', 'Torf');

-- Table: TabelleBodentemperatur
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleBodentemperatur ( 
    Monat     TEXT,
    MonatZahl INT,
    [T0-30]   REAL,
    [T30-60]  REAL,
    [T60-90]  REAL 
);

INSERT INTO [TabelleBodentemperatur] ([Monat], [MonatZahl], [T0-30], [T30-60], [T60-90]) VALUES ('Januar', 1, '4.0', '3.0', '2.0');
INSERT INTO [TabelleBodentemperatur] ([Monat], [MonatZahl], [T0-30], [T30-60], [T60-90]) VALUES ('Februar', 2, '-2.0', '1.0', '1.0');
INSERT INTO [TabelleBodentemperatur] ([Monat], [MonatZahl], [T0-30], [T30-60], [T60-90]) VALUES ('März', 3, '2.0', '2.0', '1.0');
INSERT INTO [TabelleBodentemperatur] ([Monat], [MonatZahl], [T0-30], [T30-60], [T60-90]) VALUES ('April', 4, '7.0', '4.0', '2.0');
INSERT INTO [TabelleBodentemperatur] ([Monat], [MonatZahl], [T0-30], [T30-60], [T60-90]) VALUES ('Mai', 5, '14.0', '7.0', '6.0');
INSERT INTO [TabelleBodentemperatur] ([Monat], [MonatZahl], [T0-30], [T30-60], [T60-90]) VALUES ('Juni', 6, '16.0', '12.0', '9.0');
INSERT INTO [TabelleBodentemperatur] ([Monat], [MonatZahl], [T0-30], [T30-60], [T60-90]) VALUES ('Juli', 7, '16.0', '14.0', '9.0');
INSERT INTO [TabelleBodentemperatur] ([Monat], [MonatZahl], [T0-30], [T30-60], [T60-90]) VALUES ('August', 8, '16.5', '15.0', '9.5');
INSERT INTO [TabelleBodentemperatur] ([Monat], [MonatZahl], [T0-30], [T30-60], [T60-90]) VALUES ('September', 9, '13.0', '12.0', '8.5');
INSERT INTO [TabelleBodentemperatur] ([Monat], [MonatZahl], [T0-30], [T30-60], [T60-90]) VALUES ('Oktober', 10, '7.5', '8.0', '7.0');
INSERT INTO [TabelleBodentemperatur] ([Monat], [MonatZahl], [T0-30], [T30-60], [T60-90]) VALUES ('November', 11, '-1.0', '2.0', '3.0');
INSERT INTO [TabelleBodentemperatur] ([Monat], [MonatZahl], [T0-30], [T30-60], [T60-90]) VALUES ('Dezember', 12, '0.5', '0.5', '1.0');

-- Table: TabelleDüngerMineralisch
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleDüngerMineralisch ( 
    NameDuenger              TEXT,
    Gesamtstickstoffgehalt   REAL,
    Nitratstickstoffgehalt   REAL,
    Ammoniumstickstoffgehalt REAL,
    Amidstickstoffgehalt     REAL,
    Code                     TEXT,
    SortierRang              REAL 
);

INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Alzon 27', '27.0', '0.0', '0.0', '27.0', 'FE027', '5.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Ammonnitrat', '22.0', '11.0', '11.0', '0.0', 'FE001', '7.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Ammonnitrat-Harnstoff-Lösung', '28.0', '7.0', '7.0', '14.0', 'FE010', '8.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Ammonsulfatsalpeter', '26.0', '6.5', '19.5', '0.0', 'FE003', '9.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Basammon', '25.0', '0.0', '0.0', '25.0', 'FE028', '10.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Borammonsulfatsalpeter', '26.0', '6.5', '19.5', '0.0', 'FE029', '11.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Diammonphosphat', '18.0', '0.0', '18.0', '0.0', 'FE006', '6.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Harnstoff', '46.0', '0.0', '0.0', '46.0', 'FE005', '5.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Kalkammonsalpeter', '27.0', '13.5', '13.5', '0.0', 'FE011', '1.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Kalksalpeter', '15.5', '15.5', '0.0', '0.0', 'FE008', '12.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Kalkstickstoff gemahlen', '20.5', '0.0', '0.0', '20.5', 'FE030', '13.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Schwefels. Ammoniak', '21.0', '0.0', '21.0', '0.0', 'FE004', '14.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Stickstoffmagnesia', '22.0', '11.0', '11.0', '0.0', 'FE035', '15.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('NP Stickstoffphosphat', '20.0', '0.0', '20.0', '0.0', 'FE007', '5.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('wasserfreies Ammoniak', '82.0', '0.0', '82.0', '0.0', 'FE009', '16.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('NPK Comp. Plus', '15.0', '7.5', '7.5', '0.0', 'FE031', '4.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('NPK Kampka', '6.0', '3.0', '3.0', '0.0', 'FE032', '17.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('NPK Kampka Enpeka', '10.0', '5.0', '5.0', '0.0', 'FE033', '18.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('NPK Nitrop.', '13.0', '6.5', '6.5', '0.0', 'FE034', '19.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Ammonium Polyphoshat', '11.0', '0.0', '11.0', '0.0', 'FE012', '30.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Superphosphat', '0.0', '0.0', '0.0', '0.0', 'FE013', '31.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Triple-Superphophat', '0.0', '0.0', '0.0', '0.0', 'FE014', '32.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Liquid Phosphoric Acid', '0.0', '0.0', '0.0', '0.0', 'FE015', '33.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('40er Kali, Standard', '0.0', '0.0', '0.0', '0.0', 'FE016', '34.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('NK-Dünger', '22.0', '22.0', '0.0', '0.0', 'FE017', '29.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Kaliumsulfat', '0.0', '0.0', '0.0', '0.0', 'FE018', '35.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Harnstoff granuliert', '46.0', '0.0', '0.0', '46.0', 'FE019', '28.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Phosphathaltiges Gestein', '0.0', '0.0', '0.0', '0.0', 'FE021', '36.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Dolomitischer Kalkstein', '0.0', '0.0', '0.0', '0.0', 'FE020', '37.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Calcitic Kalkstein', '0.0', '0.0', '0.0', '0.0', 'FE021', '38.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Rhizobien', '20.0', '10.0', '10.0', '0.0', 'FE024', '28.0');
INSERT INTO [TabelleDüngerMineralisch] ([NameDuenger], [Gesamtstickstoffgehalt], [Nitratstickstoffgehalt], [Ammoniumstickstoffgehalt], [Amidstickstoffgehalt], [Code], [SortierRang]) VALUES ('Kohelnsaurer Kalk', '0.0', '0.0', '0.0', '0.0', 'FE026', '39.0');

-- Table: TabelleDüngerOrganisch
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleDüngerOrganisch ( 
    NameDuenger            TEXT,
    Trockensubstanz        REAL,
    Gesamtstickstoffgehalt REAL,
    Ammoniumstickstoff     REAL,
    OrganischeSubstanz     REAL,
    Code                   TEXT,
    SortierRang            REAL 
);

INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rimikomp (LA:10%, HA:60%) (1993, Kloster)', null, null, null, null, 'LA:10%, HA:60%) (1993, Kloster)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rimi (LA:10%, HA:60%)', null, null, null, null, 'LA:10%, HA:60%)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Stroh (LA:70%)', null, null, null, null, 'LA:70%)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Zwifru Senf (LA:90%)', null, null, null, null, 'LA:90%)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Zwifru. Ölrettich  (LA:90%)', null, null, null, null, 'LA:90%)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Zwifru. Weißklee (LA:100%)', null, null, null, null, 'LA:100%)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('SB-Stroh (LA:70%)', null, null, null, null, 'LA:70%)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Schwgü (LA:10%, HA:30%) (93, A6)', null, null, null, null, 'LA:10%, HA:30%) (93, A6)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Zwifru Vielfachmisch.  (LA:90%)', null, null, null, null, 'LA:90%)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Pflanzenrückstände', '15.0', '0.5', '0.1', '15.0', 'RE001', '23.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Grünmasse', '15.0', '0.5', '0.1', '15.0', 'RE002', '24.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Laufhof-Gülle', '25.0', '0.7', '0.4', '16.0', 'RE003', '25.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Flüssigmist', '10.0', '0.7', '0.45', '7.3', 'RE004', '26.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindergülle (5%TS)', '5.0', '0.23', '0.13', '3.59', 'RE005', '3.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindergülle (10%TS)', '10.0', '0.46', '0.25', '7.19', 'RE006', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindergülle (15%TS)', '15.0', '0.69', '0.39', '10.78', 'RE007', '2.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Schweinegülle (3,5%TS)', '3.5', '0.29', '0.2', '2.47', 'RE008', '8.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Schweinegülle (7%TS)', '7.0', '0.57', '0.4', '4.95', 'RE009', '9.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Schweinegülle (10,5%TS)', '10.5', '0.86', '0.6', '7.43', 'RE010', '6.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Schweinegülle (14%TS)', '14.0', '1.14', '0.8', '9.9', 'RE011', '7.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Geflügelgülle (7%TS)', '7.0', '0.49', '0.34', '4.47', 'RE012', '14.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Geflügelgülle (14%TS)', '14.0', '0.98', '0.68', '8.94', 'RE013', '12.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Geflügelgülle (21%TS)', '21.0', '1.47', '1.02', '13.41', 'RE014', '13.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindermist', '25.0', '0.5', '0.05', '23.0', 'RE015', '5.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Schweinemist', '25.0', '0.7', '0.07', '18.0', 'RE016', '11.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Hähnchenmist', '44.0', '2.8', '0.28', '35.0', 'RE017', '15.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Putenmist', '41.0', '2.3', '0.23', '35.0', 'RE018', '18.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Pferdemist', '25.0', '0.6', '0.06', '20.0', 'RE019', '20.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Kartoffelrückst. (LA:80%)', null, null, null, null, 'LA:80%)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rigü (LA:10%, HA:30%)', null, null, null, null, 'LA:10%, HA:30%)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('SB-Stroh (LA:80%)', null, null, null, null, 'LA:80%)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rigü (LA:10%, HA:30%) (1996; A18, A21)', null, null, null, null, 'LA:10%, HA:30%) (1996; A18, A21)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Schwgü (LA:10%, HA:40%) (93, A6)', null, null, null, null, 'LA:10%, HA:40%) (93, A6)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Schafmist', '25.0', '0.8', '0.08', '20.0', 'RE020', '19.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Hühnerfrischkot', '22.5', '1.3', '0.65', '3.0', 'RE021', '16.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Hühnertrockenkot', '50.0', '2.8', '1.4', '3.0', 'RE022', '17.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rinderjauche', '1.0', '0.42', '0.32', '0.1', 'RE023', '4.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Schweinejauche', '0.5', '0.65', '0.59', '0.05', 'RE024', '10.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Klärschlamm (5%TS)', '5.0', '0.58', '0.06', '3.5', 'RE025', '22.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Klärschlamm (35%TS)', '35.0', '0.23', '0.01', '17.0', 'RE026', '21.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Grüngutkompost', '57.0', '0.168', '0.01', '29.8', 'RE027', '23.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Mastbullengülle', '7.5', '0.4', '0.2', '5.8', 'RE028', '4.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Mastschweingülle', '7.5', '0.6', '0.4', '5.5', 'RE029', '9.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindergülle (1996, A15, A17, A19)', '4.6', '0.19', '0.1', '3.2', 'RE030', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindergülle (1996,  A17)', '7.4', '0.25', '0.1', '6.0', 'RE031', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindergülle (1996,  A19)', '7.9', '0.26', '0.11', '6.4', 'RE032', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindergülle (1996,  A2, A3, A6, A8, A11, A12)', '6.8', '0.2', '0.05', '5.4', 'RE033', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindermistkompost  (1996)', '30.6', '0.61', '0.09', '28.0', 'RE034', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindergülle (1993, Preitsameter)', '8.1', '0.33', '0.15', '6.6', 'RE035', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindergülle (1993, Müller)', '7.4', '0.39', '0.19', '6.0', 'RE036', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Schweinegülle (1993, Klostergut, alt)', '3.1', '0.1', '0.02', '2.4', 'RE037', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Mist (1993, Ri/Schw., angerottet)', '21.5', '0.57', '0.05', '20.0', 'RE038', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindermistkompost (1993, Kloster)', '39.0', '0.69', '0.01', '36.0', 'RE039', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindermist (1994, Kloster)', '23.1', '0.62', '0.04', '21.0', 'RE040', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindergülle (1994, Preitsameter ?)', '4.0', '0.18', '0.11', '2.8', 'RE041', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindermistkompost  (1995)', '29.1', '1.0', '0.02', '27.0', 'RE042', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindermist (1995, frisch)', '25.6', '0.55', '0.2', '23.6', 'RE043', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindergülle (1995, Preitsameter ?)', '6.6', '0.13', '0.07', '5.4', 'RE044', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindermist (1995, Reischl/Kreitmayr)', '15.8', '0.35', '0.04', '13.0', 'RE045', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindergülle (1995, sehr dünn)', '0.9', '0.06', '0.02', '0.7', 'RE046', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindergülle (1996; A18, A21) (LA:10%)', '8.6', '0.24', '0.11', '7.0', 'RE047', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rindergülle (1995, A1, A3, A5, A7, A19) (LA:10%)', '4.0', '0.13', '0.05', '2.8', 'RE048', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Zwischenfrucht Vielfachmischung (Herbst 92) (LA:90%)', '15.0', '0.55', '0.0', '15.0', 'RE049', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Zwischenfrucht Senf (schlecht versorgt) (LA:90%)', '15.0', '0.4', '0.0', '15.0', 'RE051', '23.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Zwischenfrucht Senf (gut versorgt) (LA:90%)', '15.0', '0.6', '0.0', '15.0', 'RE052', '23.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Zwischenfrucht Sommerwicken (LA:90%)', '15.0', '0.8', '0.0', '15.0', 'RE050', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Schweinegülle (1993, A6) (LA:10%)', '3.0', '0.15', '0.03', '2.4', 'RE053', '1.0');
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rigü (LA:10%, HA:30%) (Müller)', null, null, null, null, 'LA:10%, HA:30%) (Müller)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Rigü (LA:10%, HA:30%) (1995, Preits.)', null, null, null, null, 'LA:10%, HA:30%) (1995, Preits.)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Zwifru So.wicken (LA:90%)', null, null, null, null, 'LA:90%)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Schwgü (LA:10%, HA:30%)(1993, A6)', null, null, null, null, 'LA:10%, HA:30%)(1993, A6)', null);
INSERT INTO [TabelleDüngerOrganisch] ([NameDuenger], [Trockensubstanz], [Gesamtstickstoffgehalt], [Ammoniumstickstoff], [OrganischeSubstanz], [Code], [SortierRang]) VALUES ('Zwifru. So.wicken (LA:100%)', null, null, null, null, 'LA:100%)', null);

-- Table: TabelleErnterückstände
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleErnterückstände ( 
    Fruchtart           TEXT,
    Emit                REAL,
    FobAbgefahren       REAL,
    FobNichtAbgefahren  REAL,
    Fut                 REAL,
    CnErnterueckstaende REAL,
    CnWurzel            REAL,
    PRIMARY KEY ( Fruchtart ) 
);

INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Ackerbohnen', '37.0', null, '0.86', '0.23', '67.0', '16.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Ackersenf', '310.0', null, '0.13', '0.05', '40.0', '46.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Dinkel', '50.0', '0.2', '0.86', '0.45', '75.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Durum', '50.0', '0.2', '0.86', '0.45', '75.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Erdbeeren', '95.0', '0.1', '1.0', '0.01', '33.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Futtererbse', '36.0', '0.1', '1.21', '0.23', '67.0', '16.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Futterrueben', '500.0', '0.1', '0.13', '0.02', '25.0', '10.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Hafer', '45.0', '0.2', '1.03', '0.56', '75.0', '19.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Hanf', '15.0', '0.05', '3.8', '0.01', '50.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Hopfen', '15.0', '0.1', '0.95', '0.25', '50.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Kartoffeln', '342.0', null, '0.1', '0.03', '40.0', '10.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Klee', '370.0', '0.01', '0.2', '0.04', '15.0', '15.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Kleegras', '370.0', '0.01', '0.11', '0.05', '15.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Koernermais', '65.0', null, '1.12', '0.26', '55.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Luzerne', '400.0', '0.01', '0.2', '0.03', '15.0', '12.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Moehren', '350.0', '0.1', '0.34', '0.01', '50.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Oelrettich', '410.0', null, '0.11', '0.05', '36.0', '46.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Phacelia', '520.0', null, '0.08', '0.03', '33.0', '46.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Porree', '350.0', '0.1', '0.56', '0.01', '33.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Rote Beete', '300.0', '0.1', '0.6', '0.01', '33.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Silomais', '400.0', '0.28', null, '0.04', '60.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Sommergerste', '45.0', '0.2', '0.95', '0.5', '75.0', '22.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Sommerraps', '25.0', null, '0.86', '0.55', '70.0', '17.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Sommerweizen', '60.0', '0.2', '0.86', '0.39', '75.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Sommerwicke', '14.0', '0.1', '1.29', '0.23', '15.0', '16.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Sonnenblumen', '32.0', '0.24', '1.2', '0.65', '50.0', '30.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Spinat', '150.0', '0.1', '0.6', '0.01', '25.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Triticale', '60.0', '0.2', '0.95', '0.45', '75.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Weißkraut', '600.0', '0.1', '0.6', '0.01', '33.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Welsches Weidelgras', '300.0', '0.02', '0.15', '0.08', '29.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Wintergerste', '60.0', '0.2', '0.86', '0.5', '75.0', '19.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Winterraps', '30.0', null, '1.4', '0.65', '70.0', '17.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Winterroggen', '47.0', '0.2', '1.29', '0.4', '75.0', '20.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Winterrübsen', '30.0', null, '1.3', '0.65', '31.0', '87.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Winterweizen', '67.0', '0.2', '0.77', '0.39', '75.0', '21.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Winterwicke', '12.0', '0.1', '1.29', '0.23', '15.0', '16.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Zuckerrüben', '574.0', '0.1', '0.13', '0.02', '27.0', '10.0');
INSERT INTO [TabelleErnterückstände] ([Fruchtart], [Emit], [FobAbgefahren], [FobNichtAbgefahren], [Fut], [CnErnterueckstaende], [CnWurzel]) VALUES ('Zwiebeln', '350.0', '0.1', '0.4', '0.01', '33.0', '20.0');

-- Table: TabelleExposition
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleExposition ( 
    Exposition     TEXT,
    ExpositionLang TEXT 
);

INSERT INTO [TabelleExposition] ([Exposition], [ExpositionLang]) VALUES ('[keine]', 'keine');
INSERT INTO [TabelleExposition] ([Exposition], [ExpositionLang]) VALUES ('N', 'Nord');
INSERT INTO [TabelleExposition] ([Exposition], [ExpositionLang]) VALUES ('NE', 'Nord-Ost');
INSERT INTO [TabelleExposition] ([Exposition], [ExpositionLang]) VALUES ('E', 'Ost');
INSERT INTO [TabelleExposition] ([Exposition], [ExpositionLang]) VALUES ('SE', 'Süd-Ost');
INSERT INTO [TabelleExposition] ([Exposition], [ExpositionLang]) VALUES ('S', 'Süd');
INSERT INTO [TabelleExposition] ([Exposition], [ExpositionLang]) VALUES ('SW', 'Süd-West');
INSERT INTO [TabelleExposition] ([Exposition], [ExpositionLang]) VALUES ('W', 'West');
INSERT INTO [TabelleExposition] ([Exposition], [ExpositionLang]) VALUES ('NW', 'Nord-West');

-- Table: TabelleFkGpvPwp2
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleFkGpvPwp2 ( 
    ton TEXT,
    os  REAL,
    nFK REAL,
    FK  REAL,
    GPV REAL 
);

INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' < 5', '2.0', '2.0', '1.0', '1.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' < 5', '4.0', '4.0', '3.0', '2.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' < 5', '6.0', '7.0', '7.0', '5.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' < 5', '8.0', '10.0', '12.0', '9.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' < 5', '10.0', '12.0', '15.0', '11.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' < 5', '12.0', '14.0', '19.0', '14.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' < 5', '14.0', '16.0', '23.0', '17.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 5 - 12', '2.0', '1.0', '1.0', '1.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 5 - 12', '4.0', '3.0', '3.0', '2.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 5 - 12', '6.0', '5.0', '6.0', '4.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 5 - 12', '8.0', '8.0', '10.0', '8.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 5 - 12', '10.0', '10.0', '13.0', '10.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 5 - 12', '12.0', '13.0', '17.0', '13.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 5 - 12', '14.0', '15.0', '21.0', '16.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 12 - 17', '4.0', '2.0', '2.0', '1.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 12 - 17', '6.0', '5.0', '6.0', '4.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 12 - 17', '8.0', '8.0', '9.0', '7.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 12 - 17', '10.0', '10.0', '12.0', '9.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 12 - 17', '12.0', '12.0', '15.0', '11.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 12 - 17', '14.0', '15.0', '20.0', '15.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 17 - 35', '4.0', '1.0', '1.0', '0.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 17 - 35', '6.0', '4.0', '4.0', '3.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 17 - 35', '8.0', '6.0', '7.0', '6.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 17 - 35', '10.0', '8.0', '9.0', '8.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 17 - 35', '12.0', '10.0', '12.0', '11.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 17 - 35', '14.0', '12.0', '15.0', '14.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 35 - 65', '6.0', '2.0', '2.0', '2.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 35 - 65', '8.0', '3.0', '4.0', '5.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 35 - 65', '10.0', '4.0', '5.0', '8.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 35 - 65', '12.0', '6.0', '7.0', '11.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' 35 - 65', '14.0', '8.0', '9.0', '13.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' > 65', '6.0', '2.0', '2.0', '2.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' > 65', '8.0', '3.0', '3.0', '5.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' > 65', '10.0', '4.0', '4.0', '8.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' > 65', '12.0', '5.0', '5.0', '11.0');
INSERT INTO [TabelleFkGpvPwp2] ([ton], [os], [nFK], [FK], [GPV]) VALUES (' > 65', '14.0', '6.0', '6.0', '13.0');

-- Table: TabelleFruchtarten
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleFruchtarten ( 
    tFruchtart                  TEXT,
    Code                        TEXT,
    AehrenRispenFahnenSchieben  INT,
    Reife                       INT,
    Pflanzenlaenge              INT,
    Auswinterung                INT,
    Lagerneigung                INT,
    Kornertrag                  INT,
    Halmknicken                 INT,
    Aehrenknicken               INT,
    Spelzenanteil               INT,
    FaoZahl                     INT,
    Spindeldicke                INT,
    Bluehbeginn                 INT,
    Oelertrag                   INT,
    Oelgehalt                   INT,
    Blattanteil                 INT,
    Bluehzeitpunkt              INT,
    Ploidie                     INT,
    Zuckerertrag                INT,
    Zuckergehalt                INT,
    Aminostickstoff             INT,
    Bestandesdichte             INT,
    KornZahlAehreRispe          INT,
    Tausenkorngewicht           INT,
    Qualitaetsgruppe            INT,
    ReifeverzoegerungStroh      INT,
    NeigungBestockung           INT,
    KaelteempfindlichkeitJugend INT,
    Kolbenansatzhoehe           INT,
    Gesamttrockenmasse          INT,
    Kolbentrockenmasse          INT,
    Trockenkolbenanteil         INT,
    NeigungBluehen              INT,
    MassenbildungAnfang         INT,
    Rohproteinertrag            INT,
    BestandeshoeheBluete        INT,
    NeigungSchossen             INT,
    Ruebenfrischmasse           INT,
    Ruebentrockenmasse          INT,
    Ruebentrockensubstanzgehalt INT,
    SitzBoden                   INT,
    PRIMARY KEY ( tFruchtart ) 
);

INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Ackerbohnen', 'FB', 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Ackersenf', 'WM', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Brache', 'FA', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Buche', 'BW', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Dinkel', 'GH', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Durum', 'DH', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Erdbeeren', 'SY', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Fichte', 'FW', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Futtererbse', 'GP', 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Futterrueben', 'BF', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Hafer', 'OA', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Hanf', 'IH', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Hopfen', 'HP', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Kartoffeln', 'PT', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Klee', 'CV', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Kleegras', 'GC', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Koernermais', 'MZ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Luzerne', 'AL', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Moehren', 'CT', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Oelrettich', 'OR', 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Phacelia', 'CB', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Porree', 'LK', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Rote Beete', 'RB', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Silomais', 'MZ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Sommergerste', 'S', 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Sommerraps', 'TC', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Sommerroggen', 'SR', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Sommerweizen', 'SW', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Sommerwicke', 'SV', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Sonnenblumen', 'SF', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Spinat', 'SH', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Triticale', 'TC', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Weißkraut', 'WC', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Welsches Weidelgras', 'IR', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Wintergerste', 'BA', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Winterraps', 'RP', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Winterroggen', 'RY', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Winterrübsen', 'BR', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Winterweizen', 'WH', 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Winterwicke', 'WV', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Zuckerrüben', 'BS', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO [TabelleFruchtarten] ([tFruchtart], [Code], [AehrenRispenFahnenSchieben], [Reife], [Pflanzenlaenge], [Auswinterung], [Lagerneigung], [Kornertrag], [Halmknicken], [Aehrenknicken], [Spelzenanteil], [FaoZahl], [Spindeldicke], [Bluehbeginn], [Oelertrag], [Oelgehalt], [Blattanteil], [Bluehzeitpunkt], [Ploidie], [Zuckerertrag], [Zuckergehalt], [Aminostickstoff], [Bestandesdichte], [KornZahlAehreRispe], [Tausenkorngewicht], [Qualitaetsgruppe], [ReifeverzoegerungStroh], [NeigungBestockung], [KaelteempfindlichkeitJugend], [Kolbenansatzhoehe], [Gesamttrockenmasse], [Kolbentrockenmasse], [Trockenkolbenanteil], [NeigungBluehen], [MassenbildungAnfang], [Rohproteinertrag], [BestandeshoeheBluete], [NeigungSchossen], [Ruebenfrischmasse], [Ruebentrockenmasse], [Ruebentrockensubstanzgehalt], [SitzBoden]) VALUES ('Zwiebeln', 'ON', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

-- Table: TabelleGeräteBodenbearbeitung
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleGeräteBodenbearbeitung ( 
    NameGeraet  TEXT,
    Code        TEXT,
    SortierRang INT,
    PRIMARY KEY ( NameGeraet ) 
);

INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Beetpflug', 'TI004', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Cambridge-Walze', 'TI039', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Crosskill-Walze', 'TI040', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Dammfräse', 'TI044', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Federzinkenegge', 'TI012', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Feingrubber', 'TI037', 1);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Fräse', 'TI042', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Kreiselegge', 'TI034', 1);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Löffelegge', 'TI033', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Mulchgerät', 'TI046', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Ringelwalze', 'TI041', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Rotortiller', 'TI015', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Rüttelegge', 'TI035', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Saategge', 'TI013', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Schälgrubber', 'TI010', 1);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Scheibenegge', 'TI031', 1);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Scheibenpflug', 'TI007', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Schichtengrubber', 'TI030', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Schleppegge', 'TI038', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Schwergrubber', 'TI029', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Spatenmaschine', 'TI027', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Spatenrollegge', 'TI032', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Striegelegge', 'TI036', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Tiefenmeißel', 'TI045', 2);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Volldrehpflug', 'TI005', 1);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Volldrehpflug mit Packer', 'TI025', 1);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Volldrehpflug mit Untergrundlockerer', 'TI024', 1);
INSERT INTO [TabelleGeräteBodenbearbeitung] ([NameGeraet], [Code], [SortierRang]) VALUES ('Zinkenrotor', 'TI043', 2);

-- Table: TabelleGrobboden
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleGrobboden ( 
    VolProzentBereich TEXT,
    VolProzentWert    REAL,
    GewProzentBereich TEXT,
    GewProzentWert    REAL,
    Bezeichnung       TEXT 
);

INSERT INTO [TabelleGrobboden] ([VolProzentBereich], [VolProzentWert], [GewProzentBereich], [GewProzentWert], [Bezeichnung]) VALUES ('< 1 (Vol.-%)', '0.5', '< 2', '1.5', 'sehr schwach steinig');
INSERT INTO [TabelleGrobboden] ([VolProzentBereich], [VolProzentWert], [GewProzentBereich], [GewProzentWert], [Bezeichnung]) VALUES ('1 - 10 (Vol.-%)', '5.0', '2 - 15', '8.0', 'schwach steinig');
INSERT INTO [TabelleGrobboden] ([VolProzentBereich], [VolProzentWert], [GewProzentBereich], [GewProzentWert], [Bezeichnung]) VALUES ('10 - 30 (Vol.-%)', '20.0', '15 - 45', '30.0', 'mittel steinig');
INSERT INTO [TabelleGrobboden] ([VolProzentBereich], [VolProzentWert], [GewProzentBereich], [GewProzentWert], [Bezeichnung]) VALUES ('30 - 50 (Vol.-%)', '40.0', '45 - 60', '53.0', 'stark steinig');
INSERT INTO [TabelleGrobboden] ([VolProzentBereich], [VolProzentWert], [GewProzentBereich], [GewProzentWert], [Bezeichnung]) VALUES ('50 - 75 (Vol.-%)', '63.0', '60 - 85', '73.0', 'sehr stark steinig');
INSERT INTO [TabelleGrobboden] ([VolProzentBereich], [VolProzentWert], [GewProzentBereich], [GewProzentWert], [Bezeichnung]) VALUES ('> 75 (Vol.-%)', '80.0', '> 85', '90.0', 'Skelettboden');

-- Table: TabelleHangneigung
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleHangneigung ( 
    HangneigungGradBereich TEXT,
    HangneigungGradWert    REAL,
    HangneigungProzent     TEXT,
    HangneigungWert        INT,
    Bezeichnung            TEXT 
);

INSERT INTO [TabelleHangneigung] ([HangneigungGradBereich], [HangneigungGradWert], [HangneigungProzent], [HangneigungWert], [Bezeichnung]) VALUES ('< 0.5', '0.0', '< 1', 0, 'nicht geneigt');
INSERT INTO [TabelleHangneigung] ([HangneigungGradBereich], [HangneigungGradWert], [HangneigungProzent], [HangneigungWert], [Bezeichnung]) VALUES ('0.5 - 1.0', '0.8', '1 - 2', 2, 'fast nicht geneigt');
INSERT INTO [TabelleHangneigung] ([HangneigungGradBereich], [HangneigungGradWert], [HangneigungProzent], [HangneigungWert], [Bezeichnung]) VALUES ('1 - 2', '1.5', '2 - 3.5', 3, 'sehr schwach geneigt');
INSERT INTO [TabelleHangneigung] ([HangneigungGradBereich], [HangneigungGradWert], [HangneigungProzent], [HangneigungWert], [Bezeichnung]) VALUES ('2 - 3', '2.5', '3.5 - 5', 4, 'kaum schwach geneigt');
INSERT INTO [TabelleHangneigung] ([HangneigungGradBereich], [HangneigungGradWert], [HangneigungProzent], [HangneigungWert], [Bezeichnung]) VALUES ('3 - 5', '4.0', '5 - 9', 7, 'mäßig schwach geneigt');
INSERT INTO [TabelleHangneigung] ([HangneigungGradBereich], [HangneigungGradWert], [HangneigungProzent], [HangneigungWert], [Bezeichnung]) VALUES ('5 - 7', '6.0', '9 - 12', 10, 'schwach mittel geneigt');
INSERT INTO [TabelleHangneigung] ([HangneigungGradBereich], [HangneigungGradWert], [HangneigungProzent], [HangneigungWert], [Bezeichnung]) VALUES ('7 - 10', '8.5', '12 - 18', 15, 'stark mittel geneigt');
INSERT INTO [TabelleHangneigung] ([HangneigungGradBereich], [HangneigungGradWert], [HangneigungProzent], [HangneigungWert], [Bezeichnung]) VALUES ('10 - 15', '12.5', '18 - 27', 22, 'stark geneigt');
INSERT INTO [TabelleHangneigung] ([HangneigungGradBereich], [HangneigungGradWert], [HangneigungProzent], [HangneigungWert], [Bezeichnung]) VALUES ('15 - 20', '17.5', '27 - 36', 32, 'sehr stark geneigt');
INSERT INTO [TabelleHangneigung] ([HangneigungGradBereich], [HangneigungGradWert], [HangneigungProzent], [HangneigungWert], [Bezeichnung]) VALUES ('20 - 30', '25.0', '36 - 58', 47, 'steil');
INSERT INTO [TabelleHangneigung] ([HangneigungGradBereich], [HangneigungGradWert], [HangneigungProzent], [HangneigungWert], [Bezeichnung]) VALUES ('> 30', '35.0', '> 58', 60, 'sehr steil');


-- Table: TabelleKAK
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleKAK ( 
    KAKBereich  TEXT,
    KAKWert     INT,
    Bezeichnung TEXT 
);

INSERT INTO [TabelleKAK] ([KAKBereich], [KAKWert], [Bezeichnung]) VALUES ('< 30', 20, 'sehr gering');
INSERT INTO [TabelleKAK] ([KAKBereich], [KAKWert], [Bezeichnung]) VALUES ('30 - 40', 35, 'gering');
INSERT INTO [TabelleKAK] ([KAKBereich], [KAKWert], [Bezeichnung]) VALUES ('40 - 50', 45, 'mittel');
INSERT INTO [TabelleKAK] ([KAKBereich], [KAKWert], [Bezeichnung]) VALUES ('50 - 60', 55, 'hoch');
INSERT INTO [TabelleKAK] ([KAKBereich], [KAKWert], [Bezeichnung]) VALUES ('> 60', 65, 'sehr hoch');
-- Table: TabelleLagerungsdichte
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleLagerungsdichte ( 
    LagerungsdichteBereich TEXT,
    LagerungsdichteWert    REAL,
    Bezeichnung            TEXT 
);

INSERT INTO [TabelleLagerungsdichte] ([LagerungsdichteBereich], [LagerungsdichteWert], [Bezeichnung]) VALUES ('< 1.20', '0.6', 'sehr gering');
INSERT INTO [TabelleLagerungsdichte] ([LagerungsdichteBereich], [LagerungsdichteWert], [Bezeichnung]) VALUES ('1.20 - 1.40', '1.3', 'gering');
INSERT INTO [TabelleLagerungsdichte] ([LagerungsdichteBereich], [LagerungsdichteWert], [Bezeichnung]) VALUES ('1.40 - 1.75', '1.55', 'mittel');
INSERT INTO [TabelleLagerungsdichte] ([LagerungsdichteBereich], [LagerungsdichteWert], [Bezeichnung]) VALUES ('1.75 - 1.95', '1.85', 'hoch');
INSERT INTO [TabelleLagerungsdichte] ([LagerungsdichteBereich], [LagerungsdichteWert], [Bezeichnung]) VALUES ('> 1.95', '2.0', 'sehr hoch');
-- Table: TabellePh
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabellePh ( 
    PhBereich   TEXT,
    PhWert      REAL,
    Bezeichnung TEXT 
);

INSERT INTO [TabellePh] ([PhBereich], [PhWert], [Bezeichnung]) VALUES ('> 11.0', '11.5', 'äußerst alkalisch');
INSERT INTO [TabellePh] ([PhBereich], [PhWert], [Bezeichnung]) VALUES ('10.0 - 11.0', '10.5', 'sehr stark alkalisch');
INSERT INTO [TabellePh] ([PhBereich], [PhWert], [Bezeichnung]) VALUES ('9.0 - 10.0', '9.5', 'stark alkalisch');
INSERT INTO [TabellePh] ([PhBereich], [PhWert], [Bezeichnung]) VALUES ('8.0 - 9.0', '8.5', 'mittel alkalisch');
INSERT INTO [TabellePh] ([PhBereich], [PhWert], [Bezeichnung]) VALUES ('7.5 - 8.0', '7.8', 'schwach alkalisch');
INSERT INTO [TabellePh] ([PhBereich], [PhWert], [Bezeichnung]) VALUES ('7.0 - 7.5', '7.3', 'sehr schwach alkalisch');
INSERT INTO [TabellePh] ([PhBereich], [PhWert], [Bezeichnung]) VALUES ('7.0', '7.0', 'neutral');
INSERT INTO [TabellePh] ([PhBereich], [PhWert], [Bezeichnung]) VALUES ('6.5 - 7.0', '6.8', 'sehr schwach sauer');
INSERT INTO [TabellePh] ([PhBereich], [PhWert], [Bezeichnung]) VALUES ('6.0 - 6.5', '6.3', 'schwach sauer');
INSERT INTO [TabellePh] ([PhBereich], [PhWert], [Bezeichnung]) VALUES ('5.0 - 6.0', '5.5', 'mittel sauer');
INSERT INTO [TabellePh] ([PhBereich], [PhWert], [Bezeichnung]) VALUES ('4.0 - 5.0', '4.5', 'stark sauer');
INSERT INTO [TabellePh] ([PhBereich], [PhWert], [Bezeichnung]) VALUES ('3.0 - 4.0', '3.5', 'sehr stark sauer');
INSERT INTO [TabellePh] ([PhBereich], [PhWert], [Bezeichnung]) VALUES ('< 3.0', '2.5', 'äußerst sauer');

-- Table: TabelleReststickstoff
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleReststickstoff ( 
    Fruchtart   TEXT,
    [No3N0-30]  REAL,
    [No3N30-60] REAL,
    [No3N60-90] REAL,
    [Nh4N0-30]  REAL,
    [Nh4N30-60] REAL,
    [Nh4N60-90] REAL,
    PRIMARY KEY ( Fruchtart ) 
);

INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Ackerbohnen', '38.0', '18.0', '21.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Ackersenf', '5.0', '5.0', '5.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Dinkel', '10.0', '5.0', '2.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Durum', '10.0', '5.0', '2.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Erdbeeren', '20.0', '15.0', '15.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Futtererbse', '30.0', '20.0', '10.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Futterrueben', '15.0', '5.0', '2.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Hafer', '17.0', '7.0', '2.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Hanf', '5.0', '5.0', '5.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Hopfen', '20.0', '10.0', '5.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Kartoffeln', '19.0', '7.0', '4.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Klee', '30.0', '20.0', '10.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Kleegras', '40.0', '30.0', '20.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Koernermais', '31.0', '15.0', '22.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Luzerne', '30.0', '20.0', '10.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Moehren', '30.0', '20.0', '10.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Oelrettich', '5.0', '5.0', '5.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Phacelia', '10.0', '5.0', '5.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Porree', '20.0', '15.0', '5.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Rote Beete', '20.0', '10.0', '5.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Sommergerste', '17.0', '7.0', '2.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Sommerraps', '20.0', '10.0', '6.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Sommerweizen', '17.0', '7.0', '2.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Sommerwicke', '20.0', '10.0', '5.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Sonnenblumen', '15.0', '10.0', '5.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Spinat', '30.0', '20.0', '20.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Triticale', '20.0', '7.0', '2.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Weißkraut', '30.0', '20.0', '10.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Welsches Weidelgras', '10.0', '5.0', '5.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Wintergerste', '27.0', '7.0', '2.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Winterraps', '26.0', '16.0', '12.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Winterroggen', '27.0', '7.0', '2.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Winterrübsen', '5.0', '5.0', '5.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Winterweizen', '27.0', '7.0', '2.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Winterwicke', '20.0', '10.0', '5.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Zuckerrüben', '19.0', '7.0', '4.0', '10.0', '5.0', '3.0');
INSERT INTO [TabelleReststickstoff] ([Fruchtart], [No3N0-30], [No3N30-60], [No3N60-90], [Nh4N0-30], [Nh4N30-60], [Nh4N60-90]) VALUES ('Zwiebeln', '30.0', '20.0', '15.0', '10.0', '5.0', '3.0');

-- Table: TabelleSorteneigenschaften
-- This table created by SQLite2009 Pro Enterprise Manager 
-- Osen Kusnadi - http://www.osenxpsuite.net 
-- Created date: 2011-12-16 14:28:50
CREATE TABLE TabelleSorteneigenschaften ( 
    Fruchtart                   TEXT,
    Sortenname                  TEXT,
    AehrenRispenFahnenSchieben  INT,
    Reife                       INT,
    Pflanzenlaenge              INT,
    Auswinterung                INT,
    Lagerneigung                INT,
    Kornertrag                  INT,
    Halmknicken                 INT,
    Aehrenknicken               INT,
    Spelzenanteil               INT,
    FaoZahl                     INT,
    Spindeldicke                INT,
    Bluehbeginn                 INT,
    Oelertrag                   INT,
    Oelgehalt                   INT,
    Blattanteil                 INT,
    Bluehzeitpunkt              INT,
    Ploidie                     INT,
    Zuckerertrag                INT,
    Zuckergehalt                INT,
    Aminostickstoff             INT,
    Bestandesdichte             INT,
    KornZahlAehreRispe          INT,
    Tausenkorngewicht           INT,
    Qualitaetsgruppe            INT,
    ReifeverzoegerungStroh      INT,
    NeigungBestockung           INT,
    KaelteempfindlichkeitJugend INT,
    Kolbenansatzhoehe           INT,
    Gesamttrockenmasse          INT,
    Kolbentrockenmasse          INT,
    Trockenkolbenanteil         INT,
    NeigungBluehen              INT,
    MassenbildungAnfang         INT,
    Rohproteinertrag            INT,
    BestandeshoeheBluete        INT,
    NeigungSchossen             INT,
    Ruebenfrischmasse           INT,
    Ruebentrockenmasse          INT,
    Ruebentrockensubstanzgehalt INT,
    SitzBoden                   INT 
);

CREATE TABLE TabelleStaunässestufen ( 
    tStaunässestufeEDV  TEXT,
    tStaunässestufeName TEXT,
    PRIMARY KEY ( tStaunässestufeEDV ) 
);

INSERT INTO [TabelleStaunässestufen] ([tStaunässestufeEDV], [tStaunässestufeName]) VALUES ('S0', 'nicht staunaß');
INSERT INTO [TabelleStaunässestufen] ([tStaunässestufeEDV], [tStaunässestufeName]) VALUES ('S1', 'sehr schwach staunaß');
INSERT INTO [TabelleStaunässestufen] ([tStaunässestufeEDV], [tStaunässestufeName]) VALUES ('S2', 'schwach staunaß');
INSERT INTO [TabelleStaunässestufen] ([tStaunässestufeEDV], [tStaunässestufeName]) VALUES ('S3', 'mittel staunaß');
INSERT INTO [TabelleStaunässestufen] ([tStaunässestufeEDV], [tStaunässestufeName]) VALUES ('S4', 'stark staunaß');
INSERT INTO [TabelleStaunässestufen] ([tStaunässestufeEDV], [tStaunässestufeName]) VALUES ('S5', 'sehr stark staunaß');
INSERT INTO [TabelleStaunässestufen] ([tStaunässestufeEDV], [tStaunässestufeName]) VALUES ('S6', 'äußerst staunaß');



