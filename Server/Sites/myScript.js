let truck_ip="localhost";
let truck_port=4242;

function myFunction() {
    document.getElementById("demo").innerHTML = "Paragraph changed.";
}
const header = document.querySelector('header');
const section = document.querySelector('section');

function askForStatus(){
    truck_ip = document.getElementById("ip");
    truck_port = document.getElementById("port");
    
  if(truck_ip == null || truck_port == null){
    document.getElementById("demo").innerHTML = "NOOOO"
  }else{
  //the url is unfortunately local but whatever
  let requestURL = 'http://'+truck_ip.value+':'+truck_port.value;
//send the request to the truck
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
}


    
}
function populate(obj) {
    //this should be made more beautiful
    document.getElementById("demo").innerHTML = "Name = "+ obj['Name']+'<br />'+'<br />' +'ID = ' + 
    obj['ID']+'<br />'+'<br />' +'ProcessID = '   +obj['ProcessID'] +'<br />'+'<br />' +'Speed = '  
    +obj['Speed'] +'<br />'+'<br />' +'Size of the platoon = '  +obj['PlatoonSize'] +'<br />'+'<br />' +'Am I connected to the master = '  +obj['MasterConnection'] +'<br />'+'<br />' +'Port = '  +obj['port']+'<br />'+'<br />' 
    +'Ip = '  +obj['ip'] +'<br />'+'<br />' +'Am I the master = '  +obj['master']   +'<br />'+'<br />' +'Probability to be a master = '  +obj['Probability'];


    //const myH1 = document.createElement('h1');
    //myH1.textContent = obj['Name'];
    //header.appendChild(myH1);
  
    //const myPara = document.createElement('p');
    //myPara.textContent = 'Hometown: ' + obj['homeTown'] + ' // Formed: ' + obj['formed'];
    //header.appendChild(myPara);
  }
function wiktor(){
  document.getElementById("demo").innerHTML = "Name = "+'<br />'+'<br />' + 'ID = '+'<br />'+'<br />'   +'ProcessID = ' +'<br />'+'<br />'  +'Speed = ' ;

}