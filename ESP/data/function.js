//глобальные пременные и фукция необходимые для работы
var xmlHttp=createXmlHttpObject();
function createXmlHttpObject(){
 if(window.XMLHttpRequest){
  xmlHttp=new XMLHttpRequest();
 }else{
  xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');
 }
 return xmlHttp;
}
var jsonResponse;

// фукция выполняется при запуске страницы, считывает данные из FileName (*.json)
function load(FileName){
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
  xmlHttp.open('PUT',FileName,true);
  xmlHttp.send(null);
  xmlHttp.onload = function(e) {
   jsonResponse=JSON.parse(xmlHttp.responseText);
   loadBlock(jsonResponse);
  } } }
// функция ищет и заменяет данные на странице из FileName
function loadBlock(data2) {
 data = document.getElementsByTagName('body')[0].innerHTML;
 var new_string;
for (var key in data2) {
 new_string = data.replace(new RegExp('{{'+key+'}}', 'g'), data2[key]);
 data = new_string;
}
 document.getElementsByTagName('body')[0].innerHTML = new_string;
}

// находит значение(value) элемента по id маркеровке
function val(id){
 var v = document.getElementById(id).value;
 return v;
}
// находит значение(key) и меняет value
function chb_setting(key){
var chbox = document.getElementById(key);
    if (chbox.value == "O") {
	chbox.value = "V";
     } else {
	chbox.value = "O";
    }
var v =chbox.value;
document.getElementsByTagName(key).innerHTML = v;
}

//3 функции отправляют GET запрос на сервер, меняют состояние кнопки.
function send_request(submit,server){
 request = new XMLHttpRequest();
 request.open("GET", server, true);
 request.send();
 save_status(submit,request);
}
function save_status(submit,request){
 old_submit = submit.value;
 request.onreadystatechange = function() {
  if (request.readyState != 4) return;
  submit.value = request.responseText;
  setTimeout(function(){
   submit.value=old_submit;
   submit_disabled(false);
  }, 1000);
 }
 submit.value = 'ждите...';
 submit_disabled(true);
}
function submit_disabled(request){
 var inputs = document.getElementsByTagName("input");
 for (var i = 0; i < inputs.length; i++) {
  if (inputs[i].type === 'submit') {inputs[i].disabled = request;}
 }
}
function toggle(target) {
 var curVal = document.getElementById(target).className;
 document.getElementById(target).className = (curVal === 'hidden') ? 'show' : 'hidden';
}

// фукция WebSocket, передача данных 
function run_wsocket() {
var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);
 connection.onopen = function () {
  connection.send('Connect ' + new Date());
 };
 connection.onerror = function (error) {
  console.log('WebSocket Error ', error);
 };
 connection.onmessage = function (e) {
  console.log('Server: ', e.data);
  var socket_data=JSON.parse(e.data);
  IdBlock(socket_data);
 }; }
// функция поиска по id элемента и замена текста, для работы с фукцией WebSocket
function IdBlock(wsdata) {
 var new_string;
for (var key in wsdata) {
 new_string = wsdata[key]; }
 document.getElementById([key]).innerHTML =  new_string;
 }