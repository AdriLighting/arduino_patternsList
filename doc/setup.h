/* AFAIRE

  construire un heritage des classe -> FAIT 
    pattern
      loop
      playlist
        programe
      
--
  control utilisateur -> A FINIR SUIVANT BESION UTILISATEUR
--  
  sauveardes/chargement FS -> AFAIRE
    playlist
    "pld": {
        "cmax": 10,
        "cnt": 4,
        "pos": 0,
        "lbl": "playlist_0",
        "lref": "full",
        "items": [
            {
                "id": 0,
                "lbl": "lbl_1",
                "ib": "Pépita",
                "ibcfg": "iref_cfg"
            },
            ......

  APIREST/export json -> A FINIR SUIVANT BESION UTILISATEUR
*/

/*

FONCTIONNEMENT ------------------------------------------------------------------------------

  LISTE DE BASE -----------------------------------------------------------------------------
  
    initialisation du nombre de liste   Program (uint8_t nbLb );
      size
    EX:
      _Program = new Program(2);

    initialisation des listes           initialize_lb(uint8_t p, const char * name, uint8_t items, const char * const * arr);
      position de la liste  
      id
      size: size of items array
      items: static const char* const items[] PROGMEM
    EX: 
      _Program->initialize_lb(0, "full",  ARRAY_SIZE(LPALLNAMES)          , LPALLNAMES);
      _Program->initialize_lb(1, "cat",   ARRAY_SIZE(LPALLNAMES_CAT)      , LPALLNAMES_CAT);

    chargment d'une des liste de base    
    EX:
      _Program->initialize(ARRAY_SIZE(LPALLNAMES), LPALLNAMES, "full", SORT_TYPE::ST_AB);

  PLAYLIST ----------------------------------------------------------------------------------

    les items des playlist correspondes aux items de la liste de base ratacher a cette playliste

    initialisation des playlist               initialize_playlist(uint8_t , const uint8_t * const &, const char ** const &);
      nb de playlist
      nb d'items par playlist
      liste de basse ratacher a la playliste
    EX:
      uint8_t plC   = 5;                                              // x5 playliste
      uint8_t iC[]  = {10, 5, 4, 3, 2};                               // nb items max par playlist
      const char * Ln[] = {"full", "cat", "null", "null", "null"};    // liste de bae attribuer pour chaque playliste
      _Program->initialize_playlist(plC, iC, Ln);

  CONTROL UTILISATEUR -----------------------------------------------------------------------
    EX:
                              command             arg1  arg2
      _Program->remote_action(RA::RA_LSET_PL,     "0",  "",   NULL);  // chargment de la playlist
      _Program->remote_action(RA::RA_PLAY_LB,     NULL);              // selection des item via la liste de base
      _Program->remote_action(RA::RA_PLAY_PL,     NULL);              // selection des item via la playlist
      _Program->remote_action(RA::RA_ITEM_NEXT,   NULL);              // selection de l'item voullu
      _Program->remote_action(RA::RA_ITEM_PREV,   NULL);              // **
      _Program->remote_action(RA::RA_ITEM_RND,    NULL);              // **
      _Program->remote_action(RA::RA_ITEM,        "0");               // **

      _Program->remote_action(RA::RA_PLAY_DELAY,  "10", "",   NULL);  // delay de rotation des items
      _Program->remote_action(RA::RA_PLAY_STOP,   NULL);              // play/stop/pause rotation des items


*/