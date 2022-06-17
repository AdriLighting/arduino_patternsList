function setup_sidebar(){
  var menu_btn  = document.querySelector("#menu-btn");
  var sidebar   = document.querySelector("#sidebar");
  var container = document.querySelector(".my-container");
  menu_btn.addEventListener("click", () => {
    sidebar.classList.toggle("active-nav");
    container.classList.toggle("active-cont");
  });

  document.addEventListener("DOMContentLoaded", function(){
    document.querySelectorAll('.side-navbar .nav-link').forEach(function(element){
      element.addEventListener('click', function (e) {
        let nextEl = element.nextElementSibling;
        let parentEl  = element.parentElement;  
        if(nextEl) {
          e.preventDefault(); 
          let mycollapse = new bootstrap.Collapse(nextEl);
          if(nextEl.classList.contains('show')){
            mycollapse.hide();
          } else {
            mycollapse.show();
            // find other submenus with class=show
            var opened_submenu = parentEl.parentElement.querySelector('.submenu.show');
            // if it exists, then close all of them
            if(opened_submenu){
              new bootstrap.Collapse(opened_submenu);
            }
          }
        }
      });
    })
  });    
    // sidebar.classList.toggle("active-nav");
    // container.classList.toggle("active-cont");   
}
function showGrp(pos) {
  if (pos == 0) {
    document.getElementById("grp_liste").style.display      = "";
    document.getElementById("grp_playlist").style.display   = "none";
    document.getElementById("grp_homme").style.display      = "none";
  }
  if (pos == 1) {
    document.getElementById("grp_homme").style.display      = "none";
    document.getElementById("grp_liste").style.display      = "none";
    document.getElementById("grp_playlist").style.display   = "";
  }   
  if (pos == 3) {
    document.getElementById("grp_homme").style.display      = "";
    document.getElementById("grp_liste").style.display      = "none";
    document.getElementById("grp_playlist").style.display   = "none";
  }       
}
function api_request(op, type, oS, oG){
  var json = {};
  json.op = op;   
  json.type = type;   

  var getter = [];
  oG.forEach(function(item, index, array) {
    getter.push(item);
  });
  var setter = {}.set = [];
  oS.forEach(function(item, index, array) {
    setter.push({ 
      "n" : item.n,
      "v" : item.v
    })
  }); 
  json.set = setter;
  json.get = getter;  
  return JSON.stringify(json) ;;
}


function setData(){
  // ROTATION
  ProgramPtr().Loop.update();

  // playlist variables
  ProgramPtr().PL_statu.update();

  // lb variables
  ProgramPtr().LB_statu.update();

  // playlist item pour modification ou affichage
  playlisItemsDispPtr().update_list();

  // playliste list items
  Playlist.update();

  if (ProgramPtr().PL_statu.set && ProgramPtr().PL_statu.play) {
    playlisItemsPtr().update_list();
    document.getElementById("cmb_lb").options.selectedIndex = ProgramPtr().PL_statu.t_pos;   
    document.getElementById("bt_pl_1").innerText    = "playlist off";
    document.getElementById("grp_pl").style.display = "";
    document.getElementById("grp_lb").style.display = "none";
  }else{
    ListBasePtr().update_list();
    document.getElementById("cmb_lb").options.selectedIndex = ProgramPtr().LB_statu.t_pos;
    document.getElementById("bt_pl_1").innerText    = "playlist on";
    document.getElementById("grp_pl").style.display = "none";           
    document.getElementById("grp_lb").style.display = "";
  } 

    if (document.getElementById("cmb_plAddItem").length == 0) {
      ListBasePtr().inputSelect_fill("cmb_plAddItem");
    }
    if (document.getElementById("cmb_plAddItem").length == 0) 
      socket_send(api_request(0, "SOKCET", [],["list_lb"]));
    else {
      if (_upd_pl_base) {
        _upd_pl_base = false;        
        PlaylistPtr().get_pl_1();
      }
    }

}

function set_debug(v){
  socket_connect_display        = v;
  socket_display                = v;
  ListBasePtr()._degug          = v;
  PlaylistPtr()._degug          = v;
  playlisItemsPtr()._degug      = v;
  playlisItemsDispPtr()._degug  = v;
  ProgramPtr()._degug           = v;    
}
function set_ctrlId(){
  PlaylistPtr().set_event();
  ListBasePtr().set_event();
  ProgramPtr().Loop.set_event();
  ProgramPtr().PL_statu.set_event();
  playlisItemsDispPtr().set_event();
}

const ListBase          = new ListBaseClass("cmb_lb");
const playlisItems      = new playlisItemstClass("cmb_lb");
const playlisItemsDisp  = new playlisItemsDisptClass("cmb_pli");
const Playlist          = new playlistClass("cmb_plld", "cmb_pll");
const Program           = new ProgramClass("table_ap", "table_pl", "table_plDisp", "table_lb");
function ListBasePtr()          {return ListBase;}
function PlaylistPtr()          {return Playlist;}
function playlisItemsPtr()      {return playlisItems;}
function playlisItemsDispPtr()  {return playlisItemsDisp;}
function ProgramPtr()           {return Program;}



set_ctrlId();

set_debug(true);

setup_sidebar();



socket_initialize();
