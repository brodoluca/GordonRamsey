let truck_ip="localhost";
let truck_port=4242;
var truck_count=0
function myFunction() {
  truck_count++;
  var x = document.createElement("truck2");

  truck_ip = document.getElementById("ip");
  truck_port = document.getElementById("port");
  console.log(truck_ip.value);
  console.log(truck_port.value);


   // var x = document.createElement("INPUT");

  // x.setAttribute("type", "image");
  // x.setAttribute("src", "trucm.jpg");
  // x.setAttribute("onclick","askForStatus()");
  // x.setAttribute("id","truck"+truck_count);
  // x.setAttribute("name","truck"+truck_count);
  // document.body.appendChild(x);


  // var y = document.createElement("LABEL");
  // var t = document.createTextNode("adsads");

  // y.setAttribute("for","truck"+truck_count);
  // y.appendChild(t);
  // document.insertBefore(y,document.getElementById("truck"+truck_count));
  // document.body.appendChild(y);

}


const header = document.querySelector('header');
const section = document.querySelector('section');

function askForStatus(){

    //the url is unfortunately local but whateve;
    
    let requestURL2 = 'http://' + truck_ip.value + ':'+truck_port.value;
    console.log(requestURL2);
    console.log(requestURL2.type);

    ///////////////////////////////////////////////////////
    requestURL = 'http://localhost:4242'
    let request = new XMLHttpRequest();
    request.open('GET', requestURL);
    request.responseType = 'json';
        //when the truck responds, shit happens
    request.onload = function() {
        console.log(request.response);
        const superHeroes = request.response;
        populate(superHeroes);
       
    }    
    request.send();
    /////////////////////////////////////////////////////
}


    
function populate(obj) {
    //this should be made more beautiful
    let a ="Name: "+ obj['Name']+', ID:' + 
    obj['ID']+', ProcessID:'   +obj['ProcessID'] +', Speed: '  
    +obj['Speed'] +', Platoonsize: '  +obj['PlatoonSize'] +', Masterconnection: '  +obj['MasterConnection'] +', port: '  +obj['port']+', ip: '  +obj['ip'] +', master: '  +obj['master']  ;
    
    
    
    
    console.log(a);
    document.getElementById("demo").innerHTML =a;
  }

  