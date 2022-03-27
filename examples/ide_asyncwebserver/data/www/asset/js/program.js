function createTable(idName, tbID, start, end, p){
    var table         = document.getElementById(tbID).getElementsByTagName('tbody')[0];
    var tableRows     = document.getElementById(tbID).getElementsByTagName('tbody')[0].rows;  
    var checkedRows   = [];
    for (var k = 0; k < tableRows.length; k++)    {checkedRows.push(tableRows[k]);}
    for (var k = 0; k < checkedRows.length; k++)  {checkedRows[k].parentNode.removeChild(checkedRows[k]);}   
    var newRow  = table.insertRow();
    for (var k = start; k < end; k++) {
      var cell    = newRow.insertCell(k);
      var handl = '<th><p id = "'+idName+''+(k+p)+'" class="lead mb-0 text-center">pos</p></th>'
      cell.innerHTML = handl;
    } 
}

let LoopClass = class {
  constructor(v) {
    this.play     = null;
    this.pause    = null;
    this.rnd      = null;
    this.delay    = null;
    this.delaymin = null;
    this.tableId  =v;
    createTable("LoopClass_", this.tableId, 0, 5, 0);
  }
  set_event(){
    var array_2 = ["APSET_ITEM_PREV","APSET_ITEM_NEXT","APSET_PLAY_START","APSET_PLAY_PAUSE","APSET_PLAY_STOP","APSET_PLAY_RND"];
    let array_1 = ["prev","next","play","pause","stop","rnd","pl"];
    let dsTable = document.querySelector("div.table-responsive.ap");
    let dTable = document.createElement('table');
    dTable.className = 'table text-center';
    let dTableBody = document.createElement('tbody');
    dTable.append(dTableBody);
    dsTable.append(dTable);
    let dTableTr = document.createElement('tr');
    dTableBody.append(dTableTr);
    var btn, dTableTd;
    for (var i = 0; i < array_1.length; i++) {
      dTableTd = document.createElement('td');
      dTableTr.append(dTableTd);      
      btn = document.createElement('button')
      btn.innerHTML = array_1[i];
      btn.type  = "button";
      btn.className  = "btn btn-primary btn-ap";
      if (array_1[i]=="pl") btn.setAttribute("id","bt_pl_1")
      else {
       btn.addEventListener('click',(e) => {
        for (var i = 0; i < array_1.length; i++) {
          if (e.target.textContent==array_1[i]) socket_send(api_request(0, "SOKCET", [{"n":array_2[i],  "v":""}], [""]));}   },false);       
      }
      dTableTd.append(btn);
      dTableTd = document.createElement('td');
      dTableTr.append(dTableTd) ;
    }
    // document.querySelectorAll('.btn-ap').forEach(option => option.addEventListener('click',(e) => {LoopClass_btEvent(e);},false));
  }
  socketParse(json) {
    if (isUndefined(json.statu)) return;
    this.play     = json.statu.play;
    this.pause    = json.statu.pause;
    this.rnd      = json.statu.rnd;
    this.delay    = json.statu.delay;
    this.delaymin = json.statu.delaymin;  
  }  
  update(){
    let array_1 = [this.play,this.pause,this.rnd,this.delay,this.delaymin];
    for (var i = 0; i < 5; i++) {document.getElementById("LoopClass_"+i).innerHTML = array_1[i];}       
  }  
}
let PL_statuClass = class {
  constructor(v1, v2) {
    this.set      = null;
    this.play     = null;
    this.pos      = null;
    this.cnt      = null;
    this.ib       = null;
    this.t_pos    = null;
    this.t_min    = null;
    this.t_max    = null;
    this.tableId  = v1;
    this.table2Id = v2;
    createTable("PL_statuClass_", this.tableId, 0, 4, 0);
    createTable("PL_statuClass_", this.table2Id, 0, 2, 4);
  }
  set_event(){
    document.getElementById("bt_pl_1").addEventListener('click', event => {
      ListBasePtr().inputSelect_reset(ListBasePtr().inpSel);    
      if (this.set && this.play)  socket_send(api_request(0, "SOKCET", [{"n":"APSET_PLAY_LB", "v":""}],  ["list_lb", "loop"]));
      else                        socket_send(api_request(0, "SOKCET", [{"n":"APSET_PLAY_PL", "v":""}],  ["list_pl", "loop"]));
    });     
  }
  socketParse(json) {
    if (isUndefined(json.pl)) return;
    this.set      = json.pl.set;
    this.play     = json.pl.play;
    this.pos      = json.pl.pos;
    this.cnt      = json.pl.cnt;
    this.ib       = json.pl.ib;
    if (isUndefined(json.plt)) return;
    this.t_pos    = json.plt.pos;
    this.t_min    = json.plt.min;
    this.t_max    = json.plt.max;
  }  
  update(){
    let array = [this.set,this.play,this.pos,this.cnt,this.ib];
    for (var i = 0; i < 5; i++) {document.getElementById("PL_statuClass_"+i).innerHTML = array[i];}
    document.getElementById("PL_statuClass_5").innerHTML = this.t_pos + "/" + this.t_max;   
  }
}
let LB_statuClass = class {
  constructor(v) {
    this.name     = null;
    this.pos      = null;
    this.cnt      = null;
    this.iname    = null;
    this.icnt     = null;
    this.icmax    = null;
    this.t_pos    = null;
    this.t_min    = null;
    this.t_max    = null; 
    this.tableId  = v;
    createTable("LB_statuClass_table_", this.tableId, 0, 7, 0);
  }
  socketParse(json) {
    if (isUndefined(json.lb)) return;
    this.name     = json.lb.name;
    this.pos      = json.lb.pos;
    this.cnt      = json.lb.cnt;
    this.iname    = json.lb.iname;
    this.icnt     = json.lb.icnt;
    this.icmax    = json.lb.icmax;
    if (isUndefined(json.lbt)) return;
    this.t_pos    = json.lbt.pos;
    this.t_min    = json.lbt.min;
    this.t_max    = json.lbt.max;   
  }  
  update(){
    let array = [this.name,this.pos,this.cnt,this.iname,this.icnt,this.icmax];
    for (var i = 0; i < 6; i++) {document.getElementById("LB_statuClass_table_"+i).innerHTML = array[i];}
    document.getElementById("LB_statuClass_table_6").innerHTML = this.t_pos + "/" + this.t_max;   
  }
}
let ProgramClass = class {
  constructor(v1, v2, v3, v4) {
    this._degug   = true;
    this.Loop     = new LoopClass(v1);
    this.PL_statu = new PL_statuClass(v2, v3);
    this.LB_statu = new LB_statuClass(v4);
  }

  socketParse(response) {
    if (response == "{\"}") return;
    if (response == "null") return;
    if (this._degug) { console.log("[ProgramClass::socketParse]"); }
    const [err, result] = safeJsonParse(response);
    if (err) {
      if (this._degug) console.log('Failed to parse JSON: ' + err.message);
      return;
    }
    var json = JSON.parse(response);  
    if (isUndefined(json.loop)) return;
    if (this._degug) {console.log(json.loop);}
    this.Loop.socketParse(json.loop);
    this.PL_statu.socketParse(json.loop);
    this.LB_statu.socketParse(json.loop);

  }
}
