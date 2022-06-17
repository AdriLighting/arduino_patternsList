let playlisItemstClass = class {
  constructor(v1) {
    this._degug   = true;
    this.lbl      = null;
    this.lref     = null;
    this.cmax     = null;
    this.cnt      = null;
    this.pos      = null;
    this.items    = [];
    this.inpSel   = v1;
  }
  inputSelect_fill(){
    var id = document.getElementById(this.inpSel);
    for (var i = 0; i < this.items.length; i++) {
      var option = document.createElement("option");
      option.text   = this.items[i].lbl;
      option.value  = i;
      id.add(option);       
    }
  }
  inputSelect_reset(){
    var id = document.getElementById(this.inpSel);
    removeOptions(id);
  }
  update_list(){
    if (this.items.length==0)return false;
    if (document.getElementById(this.inpSel).length == 0) {this.inputSelect_fill();return true;} 
    return false
  }
  get_list() {
    if (this.items.length == 0) socket_send(api_request(0, "SOKCET", [],["list_pl"]));
  }
  socketParse(response) {
    if (response == "{\"}") return;
    if (this._degug) { console.log("[playlisItemstClass::socketParse]"); }
    const [err, result] = safeJsonParse(response);
    if (err) {
      if (this._degug) console.log('Failed to parse JSON: ' + err.message);
      return;
    }
    var json = JSON.parse(response);

    if (isUndefined(json.list_pl)) return;

    if (this._degug) {console.log(json.list_pl);}

    this.lbl    = json.list_pl.lbl;
    this.lref   = json.list_pl.lref;
    this.cmax   = json.list_pl.cmax;
    this.cnt    = json.list_pl.cnt;
    this.pos    = json.list_pl.pos;
    this.items  = json.list_pl.items;
  }
}

var _upd_pl_base = false;
function plitem_new(){
  var json                = {};
  json.op                 = 1;   
  json.type               = "SOCKET";   
  var pl_item_new         = json.pl_item_new = {};
  var id_1                = document.getElementById("cmb_plAddItem");
  pl_item_new.pP          = document.getElementById("cmb_plld").options.selectedIndex;
  pl_item_new.iP          = 255;
  pl_item_new.lbl         = document.getElementById("inp_plLabel").value;
  pl_item_new.itemBase    = id_1.options[id_1.selectedIndex].text;
  pl_item_new.itemBaseCfg = id_1.options[id_1.selectedIndex].text;
  playlisItemsDispPtr().inputSelect_reset();
  socket_send(JSON.stringify(json)); 
  _upd_pl_base = true;
}
function plitem_replace(){
  var json                = {};
  json.op                 = 1;   
  json.type               = "SOCKET";   
  var pl_item_new         = json.pl_item_new = {};
  var id_1                = document.getElementById("cmb_plAddItem");
  pl_item_new.pP          = document.getElementById("cmb_plld").options.selectedIndex;
  pl_item_new.iP          = document.getElementById("cmb_pli").options.selectedIndex;
  pl_item_new.lbl         = document.getElementById("inp_plLabel").value;
  pl_item_new.itemBase    = id_1.options[id_1.selectedIndex].text;
  pl_item_new.itemBaseCfg = id_1.options[id_1.selectedIndex].text;
  pl_item_new.id          = playlisItemsDispPtr().items[pl_item_new.iP].id;
  playlisItemsDispPtr().inputSelect_reset();
  socket_send(JSON.stringify(json)); 
  _upd_pl_base = true;
}
function plitem_remove(){
  var json                = {};
  json.op                 = 1;   
  json.type               = "SOCKET";   
  var pl_item_new         = json.pl_item_remove = {};
  pl_item_new.pP          = document.getElementById("cmb_plld").options.selectedIndex;
  pl_item_new.iP          = document.getElementById("cmb_pli").options.selectedIndex;
  playlisItemsDispPtr().inputSelect_reset();
  socket_send(JSON.stringify(json)); 
  _upd_pl_base = true;
}
function pl_sav(){
  var value = document.getElementById("cmb_plld").options.selectedIndex;
  socket_send(api_request(0, "SOKCET", [{"n":"APSET_PL_TOFS","v":value}], ["", ""]));  
}
let playlisItemsDisptClass = class {
  constructor(v1) {
    this._degug   = true;
    this.lbl      = null;
    this.lref     = null;
    this.cmax     = null;
    this.cnt      = null;
    this.pos      = null;
    this.items    = [];
    this.inpSel   = v1;
  }
  set_event() {
    document.getElementById(this.inpSel).addEventListener('change', event => {
      var iP = document.getElementById(this.inpSel).options.selectedIndex;
      document.getElementById("inp_plLabel").value = this.items[iP].lbl;
      let ib = this.items[iP].ib;
      Array.from(document.querySelector("#cmb_plAddItem").options).forEach(function(id) {
          let option_text = id.text;
          let option_value = id.value;
          // let is_option_selected = id.selected;
          if (option_text== ib) {
            document.getElementById("cmb_plAddItem").options.selectedIndex =  option_value; 
          }
      });
    });
    document.getElementById("cmb_plAddItem").addEventListener('change', event => {
      var id_1 = document.getElementById("cmb_plAddItem");
      document.getElementById("inp_plLabel").value = id_1.options[id_1.selectedIndex].text;;
    });         
  }
  inputSelect_fill(){
    var id = document.getElementById(this.inpSel);
    for (var i = 0; i < this.items.length; i++) {
      var option = document.createElement("option");
      option.text   = this.items[i].lbl;
      option.value  = i;
      id.add(option);       
    }
  }
  inputSelect_reset(){
    var id = document.getElementById(this.inpSel);
    removeOptions(id);
  }
  update_list(){
    if (this.items.length==0)return false;
    if (document.getElementById(this.inpSel).length == 0) {this.inputSelect_fill();return true;} 
    return false
  }
  socketParse(response) {
    if (response == "{\"}") return;
    if (this._degug) { console.log("[playlisItemstClass::socketParse]"); }
    const [err, result] = safeJsonParse(response);
    if (err) {
      if (this._degug) console.log('Failed to parse JSON: ' + err.message);
      return;
    }
    var json = JSON.parse(response);

    if (isUndefined(json.pld)) return;

    if (this._degug) {console.log(json.pld);}

    this.lbl    = json.pld.lbl;
    this.lref   = json.pld.lref;
    this.cmax   = json.pld.cmax;
    this.cnt    = json.pld.cnt;
    this.pos    = json.pld.pos;
    this.items  = json.pld.items;

  }
}

let playlistClass = class {
  constructor(v1, v2) {
    this._degug   = true;
    this.cmax     = null;
    this.items    = [];
    this.inpSel_1 = v1;
    this.inpSel_2 = v2;     
  }
  get_pl(){
      var ptr = document.getElementById(this.inpSel_1);
      var value = ptr.options[ptr.selectedIndex].value;
      var text  = ptr.options[ptr.selectedIndex].text;  
      playlisItemsDispPtr().inputSelect_reset();
      socket_send(api_request(0, "SOKCET", [], [{"gv":{"n":"list_pld","v":value}}]));       
      // socket_send(api_request(0, "SOKCET", [{"n":"APSET_LGET_PL","v":value}], ["", ""]));       
  }
  get_pl_1(){
    if (ProgramPtr().PL_statu.set && ProgramPtr().PL_statu.play) ListBasePtr().inputSelect_reset(ListBasePtr().inpSel);
    socket_send(api_request(0, "SOKCET", [], ["", "loop", "list_pl"]));       
  }  
  set_pl(){
    var ptr   = document.getElementById(this.inpSel_2);
    var value = ptr.options[ptr.selectedIndex].value;
    var text  = ptr.options[ptr.selectedIndex].text;  
    if (ProgramPtr().PL_statu.set && ProgramPtr().PL_statu.play) ListBasePtr().inputSelect_reset(ListBasePtr().inpSel);
    socket_send(api_request(0, "SOKCET", [{"n":"APSET_LSET_PL","v":value}], ["list_pl", "loop"]));    
  }
  set_event(){
    document.getElementById(this.inpSel_1).addEventListener('change', event => {this.get_pl();});
    document.getElementById(this.inpSel_2).addEventListener('change', event => {this.set_pl();});       
  }
  inputSelect_fill(inpId){
    var id = document.getElementById(inpId);
    for (var i = 0; i < this.items.length; i++) {
      var option    = document.createElement("option");
      option.text   = this.items[i].lbl;
      option.value  = i;
      id.add(option);       
    }
  }

  inputSelect_reset(inpId){
    var id = document.getElementById(inpId);
    removeOptions(id);
  }
  update(){
    if (document.getElementById(this.inpSel_1).length == 0) this.inputSelect_fill(this.inpSel_1);
    if (document.getElementById(this.inpSel_2).length == 0) this.inputSelect_fill(this.inpSel_2);
    document.getElementById(this.inpSel_2).options.selectedIndex = Program.PL_statu.pos;  
  }

  get_list() {
    if (this.items.length == 0) socket_send(api_request(0, "SOKCET", [],["list_plsc"]));
  }
  socketParse(response) {
    if (response == "{\"}") return;
    if (this._degug) { console.log("[playlistClass::socketParse]"); }
    const [err, result] = safeJsonParse(response);
    if (err) {
      if (this._degug) console.log('Failed to parse JSON: ' + err.message);
      return;
    }
    var json = JSON.parse(response);

    if (isUndefined(json.list))     return;
    if (isUndefined(json.list.plc)) return;

    if (this._degug) {console.log(json.list);}

    this.cmax   = json.list.plc.cmax;
    this.items  = json.list.plc.items;
  }
}