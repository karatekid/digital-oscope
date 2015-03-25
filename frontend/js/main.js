var transport = new Thrift.Transport("http://localhost:9090");
var protocol  = new Thrift.Protocol(transport);
var client    = new OscopeClient(protocol);
var devInfo;
var doneDrawing = false;
try {
	devInfo   = client.ping();
	console.log(devInfo);
	client.configMeasurement({});
} catch(err) {
}

function dataToArrs(dataset) {
	var arr = [];
	for(var i = 0; i < dataset.length; ++i) {
		arr.push([i,dataset[i]]);
	}
	return arr;
}

function calc() {
	try {
		doneDrawing = false;
		var start = (new Date()).getTime();
		//Takes ~100ms for 5000 pts
		var data = client.getData();
		var dataArray = d3.range(data.vals.length).map(function(i) {
			return {x: i, y: i16AdcToDouble(data.vals[i],data.step,data.base)};
		});
		var end   = (new Date()).getTime();
		console.log((end - start) + "ms to receive " + dataArray.length + " points");
		//testUpdate(dataArray.length,(end-start));

		start   = (new Date()).getTime();
		var scale = resizeGraph(-5, 5, 0, dataArray.length);
		console.log("resized");
		graphData(dataArray,scale);
		end   = (new Date()).getTime();
		console.log((end - start) + "ms to draw " + dataArray.length + " points");
		doneDrawing = true;
	} catch(err){
		$('#result').text(err.why);
		$('#result').css('color', 'red');
	}
}

function testUpdate(n, t) {
	var r = document.createElement("tr");
	var td = document.createElement("td");
	td.innerText = n;
	r.appendChild(td);
	td = document.createElement("td");
	td.innerText = t;
	r.appendChild(td);
	document.querySelectorAll("#throughput-table")[0].appendChild(r);
}

function i16AdcToDouble(val, step, offset) {
	return val * step + offset;
}

function testThroughput(n) {
	var start = (new Date()).getTime();
	//Takes ~100ms for 5000 pts
	var testArr = client.testThroughput(n);
	var dblArr = [];
	for(var i = 0; i < testArr.length; ++i) {
		dblArr[i] = i16AdcToDouble(testArr[i],0.125, -2.5);
	}
	var end   = (new Date()).getTime();
	console.log(testArr.length + "\t" + (end - start));
	testUpdate(n, (end - start));
}

/*
   setInterval( function(){
   	if(doneDrawing){
   		calc();
   	}
   }, 500);
   */


