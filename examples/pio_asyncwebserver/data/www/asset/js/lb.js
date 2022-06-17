let ListBaseClass = class {
  constructor(v1) {
    this._degug   = null;
    this.name     = null;
    this.items    = [];
    this.cnt      = null;
    this.cntMax   = null;
    this.inpSel   = v1;
  }
  set_event(){
    document.getElementById(this.inpSel).addEventListener('change', event => {
      var ptr = document.getElementById(this.inpSel);
      var value = ptr.options[ptr.selectedIndex].value;
      var text  = ptr.options[ptr.selectedIndex].text;      
      socket_send(api_request(0, "SOKCET", [{"n":"APSET_ITEM","v":value}], [""]));     
    });
  }  
  inputSelect_id(v){this.inpSel = v}
  inputSelect_fill(v1){
    var id = document.getElementById(v1);
    for (var i = 0; i < this.items.length; i++) {
      var option = document.createElement("option");
      option.text = this.items[i];
      option.value = i;
      id.add(option);       
    }
  }
  inputSelect_reset(v1){
    var id = document.getElementById(v1);
    removeOptions(id);
  } 
  update_list(){
    if (this.items.length==0) return false;
    if (document.getElementById(this.inpSel).length == 0) {this.inputSelect_fill(this.inpSel);return true;}
    return false;
  }
  get_list() {
    if (this.items.length == 0) socket_send(api_request(0, "SOKCET", [],["list_lb"]));
  }
  socketParse(response) {
    if (response == "{\"}") return;
    if (this._degug) { console.log("[ListBaseClass::socketParse]"); }
    const [err, result] = safeJsonParse(response);
    if (err) {
      if (this._degug) console.log('Failed to parse JSON: ' + err.message);
      return;
    }
    var json = JSON.parse(response);  

    if (isUndefined(json.list_lb)) return;

    if (this._degug) {console.log(json.list_lb);}

    this.items = json.list_lb.items;
  }
}

