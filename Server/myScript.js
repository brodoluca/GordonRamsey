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
    names = ["Name", 'ID','ProcessID','Speed','PlatoonSize','MasterConnection','port','ip','master','Probability']
    //this should be made more beautiful
    // document.getElementById("demo").innerHTML = "Name = "+ obj['Name']+'<br />'+'<br />' +'ID = ' + 
    // obj['ID']+'<br />'+'<br />' +'ProcessID = '   +obj['ProcessID'] +'<br />'+'<br />' +'Speed = '  
    // +obj['Speed'] +'<br />'+'<br />' +'Size of the platoon = '  +obj['PlatoonSize'] +'<br />'+'<br />' +'Am I connected to the master = '  +obj['MasterConnection'] +'<br />'+'<br />' +'Port = '  +obj['port']+'<br />'+'<br />' 
    // +'Ip = '  +obj['ip'] +'<br />'+'<br />' +'Am I the master = '  +obj['master']   +'<br />'+'<br />' +'Probability to be a master = '  +obj['Probability'];

    var tableExists  = document.getElementById("tablet");
    
    var tbl;
    var tblBody;

    var body = document.getElementsByTagName("body")[0];
    //create table title
  
    if(typeof(tableExists) == 'undefined' || tableExists == null){
      var tbl = document.createElement("table");
      tbl.setAttribute("class", "styled-table")
      tbl.setAttribute("id", "tablet")

      var tblBody = document.createElement("tbody");
      var tblHead = document.createElement("thead");
      var rowHead = document.createElement("tr");
      
      
      
      
      for (var i = 0; i < names.length; i++) {
        var cellHead1 = document.createElement("th");
        var cellTextHead1 = document.createTextNode(names[i]);
        cellHead1.appendChild(cellTextHead1);
        rowHead.appendChild(cellHead1);
      }      
    
      tblHead.appendChild(rowHead);
      tbl.appendChild(tblHead);
    }else{
      tbl = tableExists
      a = tbl.rows.length
      b =tbl.rows[0].cells.length
      tblBody  = tbl.lastChild;
      for (var i = 0; i < a; i++) {
        for (var j = 0; j < b; j++) {
          if(tbl.rows[i].cells[1].innerHTML == obj['ID']){
            tbl.rows[i].parentNode.removeChild(tbl.rows[i]);
            j= 70;
            i = 100;
          }
        }
      }

    }
    //create ugly table
    

      // creates a table row
    var row = document.createElement("tr");
  
    for (var j = 0; j < names.length; j++) {
        // Create a <td> element and a text node, make the text
        // node the contents of the <td>, and put the <td> at
        // the end of the table row
        var cell = document.createElement("td");
        var cellText = document.createTextNode(obj[names[j]]);
        
        cell.appendChild(cellText);
        row.appendChild(cell);
      }
  
      // add the row to the end of the table body
    tblBody.appendChild(row);


    // put the <tbody> in the <table>
    tbl.appendChild(tblBody);
    // appends <table> into <body>
    body.appendChild(tbl);
     sortTable();


    //const myH1 = document.createElement('h1');
    //myH1.textContent = obj['Name'];
    //header.appendChild(myH1);
  
    //const myPara = document.createElement('p');
    //myPara.textContent = 'Hometown: ' + obj['homeTown'] + ' // Formed: ' + obj['formed'];
    //header.appendChild(myPara);
  }



  function sortTable() {
    var table, rows, switching, i, x, y, shouldSwitch;
    table = document.getElementById("tablet");
    switching = true;
    /*Make a loop that will continue until
    no switching has been done:*/
    while (switching) {
      //start by saying: no switching is done:
      switching = false;
      rows = table.rows;
      /*Loop through all table rows (except the
      first, which contains table headers):*/
      for (i = 1; i < (rows.length - 1); i++) {
        //start by saying there should be no switching:
        shouldSwitch = false;
        /*Get the two elements you want to compare,
        one from current row and one from the next:*/
        x = rows[i].getElementsByTagName("td")[1];
        y = rows[i + 1].getElementsByTagName("td")[1];
        //check if the two rows should switch place:
        if (x.innerHTML.toLowerCase() < y.innerHTML.toLowerCase()) {
          //if so, mark as a switch and break the loop:
          shouldSwitch = true;
          break;
        }
      }
      if (shouldSwitch) {
        /*If a switch has been marked, make the switch
        and mark that a switch has been done:*/
        rows[i].parentNode.insertBefore(rows[i + 1], rows[i]);
        switching = true;
      }
    }
  }



function wiktor(){
  var body = document.getElementsByTagName("body")[0];
  var tbl = document.createElement("table");
  tbl.setAttribute("class", "styled-table")
  var tblBody = document.createElement("tbody");
  var tblHead = document.createElement("thead");
  var row1 = document.createElement("tr");
  var cell1 = document.createElement("th");
  var cellText = document.createTextNode("cell in row "+9+", column "+9);
  cell1.appendChild(cellText);
  row1.appendChild(cell1);
  tblHead.appendChild(row1);
  tbl.appendChild(tblHead);
  
  for (var i = 0; i < 2; i++) {
    // creates a table row
    var row = document.createElement("tr");

    for (var j = 0; j < 2; j++) {
      // Create a <td> element and a text node, make the text
      // node the contents of the <td>, and put the <td> at
      // the end of the table row
      var cell = document.createElement("td");
      var cellText = document.createTextNode("cell in row "+i+", column "+j);
      cell.appendChild(cellText);
      row.appendChild(cell);
    }

    // add the row to the end of the table body
    tblBody.appendChild(row);
  }

  // put the <tbody> in the <table>
  tbl.appendChild(tblBody);
  // appends <table> into <body>
  body.appendChild(tbl);
  // sets the border attribute of tbl to 2;
  tbl.setAttribute("border", "2");


  


  
}