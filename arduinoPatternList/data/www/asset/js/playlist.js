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

  set_event(){
    document.getElementById(this.inpSel_1).addEventListener('change', event => {
      var ptr = document.getElementById(this.inpSel_1);
      var value = ptr.options[ptr.selectedIndex].value;
      var text  = ptr.options[ptr.selectedIndex].text;  
      playlisItemsDispPtr().inputSelect_reset();
      socket_send(api_request(0, "SOKCET", [{"n":"RA_LGET_PL","v":value}], ["", ""]));      
    });
    document.getElementById(this.inpSel_2).addEventListener('change', event => {
      var ptr = document.getElementById(this.inpSel_2);
      var value = ptr.options[ptr.selectedIndex].value;
      var text  = ptr.options[ptr.selectedIndex].text;  
      if (ProgramPtr().PL_statu.set && ProgramPtr().PL_statu.play) ListBasePtr().inputSelect_reset(ListBasePtr().inpSel);
      socket_send(api_request(0, "SOKCET", [{"n":"RA_LSET_PL","v":value}], ["list_pl", "loop"]));     
    });       
  }
  inputSelect_fill(inpId){
    var id = document.getElementById(inpId);
    for (var i = 0; i < this.items.length; i++) {
      var option = document.createElement("option");
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