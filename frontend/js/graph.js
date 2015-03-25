window.onload = function(){

  var yMin = -25,
    yMax = 25,
    xMin = 0,
    xMax = 5000;

  var data = d3.range(5000).map(function(i) {
    return {x : i, y : Math.sin(i/1000) * 25};
  });

  scale = initializeGraph(yMin, yMax, xMin, xMax);
  graphData(data, scale);

  window.onresize = function(){
    scale = resizeGraph(yMin, yMax, xMin, xMax)
    graphData(data, scale);
  }
};

function graphData(data, scale) {
  var line = d3.svg.line()
    .interpolate("linear")
    .y(function(d){ return scale.y(d.y); })
    .x(function(d){ return scale.x(d.x); });



  var timeA = (new Date()).getTime();

  var path = d3.select(".graph-path");
  if ( !path.empty() ){
    path.attr("d", function(){ return line(data); });
  }
  else {
    d3.select(".canvas").append("svg:path")
      .attr("class", "graph-path")
      .attr("d", function(){ return line(data); });
    }

  var timeB = (new Date()).getTime();
  console.log( timeB - timeA );

}

function getScale(yMin, yMax, xMin, xMax) {
  var margin = {top : 40, right : 40, bottom : 40, left : 40};
  var border = {top : 0, right : 0, bottom : 10, left : 10};

  var graphHeight = 600,
      graphWidth = window.innerWidth,
      borderBoxHeight = graphHeight - margin.top - margin.bottom,
      borderBoxWidth = graphWidth - margin.right - margin.left,
      canvasHeight = borderBoxHeight - border.top - border.bottom,
      canvasWidth = borderBoxWidth - border.right - border.left;
      
  return {
    y : d3.scale.linear()
    .domain([yMin, yMax])
    .range([0, canvasHeight]),
      
    x : d3.scale.linear()
    .domain([xMin, xMax])
    .range([0, canvasWidth])
  };
}

function initializeGraph(yMin, yMax, xMin, xMax){
  var margin = {top : 40, right : 40, bottom : 40, left : 40};
  var border = {top : 0, right : 0, bottom : 10, left : 10};

  var graphHeight = 600,
      graphWidth = window.innerWidth,
      borderBoxHeight = graphHeight - margin.top - margin.bottom,
      borderBoxWidth = graphWidth - margin.right - margin.left,
      canvasHeight = borderBoxHeight - border.top - border.bottom,
      canvasWidth = borderBoxWidth - border.right - border.left;
      
  var scale = {
    y : d3.scale.linear()
    .domain([yMin, yMax])
    .range([0, canvasHeight]),
      
    x : d3.scale.linear()
    .domain([xMin, xMax])
    .range([0, canvasWidth])
  };

  graphArea = d3.select(".graph-section")
    .style({"height" : graphHeight + "px", "width" : graphWidth + "px"});

  graphArea.select(".graph")
    .attr("height", graphHeight)
    .attr("width", graphWidth);

  var borderBox = d3.select(".border-box")
    .attr("transform", "translate(" + margin.left + "," + margin.top +")")

  borderBox.append("svg:rect")
    .attr("class", "background")
    .attr("y", 0)
    .attr("x", 0)
    .attr("height", borderBoxHeight)
    .attr("width", borderBoxWidth);

  var borderLeft = borderBox.append("svg:rect")
    .attr("class", "border-left")
    .attr("y", 0)
    .attr("x", 0)
    .attr("height", canvasHeight)
    .attr("width", border.left);

  var borderBottom = borderBox.append("svg:rect")
    .attr("class", "border-bottom")
    .attr("y", canvasHeight)
    .attr("x", border.left)
    .attr("height", border.bottom)
    .attr("width", canvasWidth);

  var canvas = borderBox.append("svg:g")
    .attr("class", "canvas")
    .attr("transform", "translate(" + border.left + "," + border.top + ")");

  var yRules = borderBox.selectAll("g.y-rule")
     .data(scale.y.ticks(10))
    .enter().append("svg:g")
      .attr("class", "y-rule")
      .attr("transform", function(d){
        return "translate(0," + (scale.y(d) + border.top) + ")";
      });

  yRules.append("svg:line")
      .attr("x1", 0)
      .attr("x2", borderBoxWidth)
      .attr("y1", 0)
      .attr("y2", 0);

  yRules.append("svg:text")
    .attr("text-anchor", "end")
    .attr("x", 0)
    .attr("y", 0)
    .attr("dx", "-.5em")
    .attr("dy", ".5em")
    .text(function(d){ return d; });

  var xRules = borderBox.selectAll("g.x-rule")
      .data(scale.x.ticks(10))
    .enter().append("svg:g")
      .attr("class", "x-rule")
      .attr("transform", function(d){
        return "translate(" + (scale.x(d) + border.left) + ",0)";
      });

  xRules.append("svg:line")
    .attr("x1", 0)
    .attr("x2", 0)
    .attr("y1", 0)
    .attr("y2", borderBoxHeight);


  xRules.append("svg:text")
    .attr("text-anchor", "middle")
    .attr("x", 0)
    .attr("y", canvasHeight)
    .attr("dy", "2em")
    .text(function(d){ return d; });

  return scale;
}

function resizeGraph (yMin, yMax, xMin, xMax){
  var margin = {top : 40, right : 40, bottom : 40, left : 40};
  var border = {top : 0, right : 0, bottom : 10, left : 10};

  var graphHeight = 600,
      graphWidth = window.innerWidth,
      borderBoxHeight = graphHeight - margin.top - margin.bottom,
      borderBoxWidth = graphWidth - margin.right - margin.left,
      canvasHeight = borderBoxHeight - border.top - border.bottom,
      canvasWidth = borderBoxWidth - border.right - border.left;
      
  var scale = {
    y : d3.scale.linear()
    .domain([yMin, yMax])
    .range([0, canvasHeight]),
      
    x : d3.scale.linear()
    .domain([xMin, xMax])
    .range([0, canvasWidth])
  };

  graphArea = d3.select(".graph-section")
    .style({"height" : graphHeight + "px", "width" : graphWidth + "px"});

  d3.select(".graph")
    .attr("height", graphHeight)
    .attr("width", graphWidth);

  var borderBox = d3.select("g.border-box")
    .attr("transform", "translate(" + margin.left + "," + margin.top +")")

  borderBox.select(".border-box > .background")
    .attr("height", borderBoxHeight)
    .attr("width", borderBoxWidth);

  borderBox.select(".border-box > .border-left")
    .attr("height", canvasHeight)
    .attr("width", border.left);

  borderBox.select(".border-box > .border-bottom")
    .attr("y", canvasHeight)
    .attr("x", border.left)
    .attr("height", border.bottom)
    .attr("width", canvasWidth)

  var canvas = borderBox.select(".canvas")
    .attr("transform", "translate(" + border.left + "," + border.top + ")");

  var yRules = borderBox.selectAll("g.y-rule")
    .attr("transform", function(d){
      return "translate(0," + (scale.y(d) + border.top) + ")";
    });

  yRules.select("line")
    .attr ("x2", borderBoxWidth);

  var xRules = borderBox.selectAll("g.x-rule")
    .attr("transform", function(d){
      return "translate(" + (scale.x(d) + border.left) + ",0)";
    });

  xRules.select("line")
    .attr("y2", borderBoxHeight);

  return scale;
}
