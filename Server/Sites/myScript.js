
function myFunction() {
    document.getElementById("demo").innerHTML = "Paragraph changed.";
}
const header = document.querySelector('header');
const section = document.querySelector('section');

function askForStatus(){

    //the url is unfortunately local but whatever
    let requestURL = 'http://localhost:4242'
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
    

    function hand () {
        console.log(this);
}


    
}
function populate(obj) {
    //this should be made more beautiful
    document.getElementById("demo").innerHTML = "wiktor"+'\n' + 'ID'+'\n'   +'ProcessID' +'\n'  +'Speed' ;


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