var Risk = {

	/**
	 * Settings Object, holding application wide settings
	 */
	Settings :{
		globalScale: 1,
		colors: {yellow: '#ff0', green: '#0f0', blue: '#00f', red: '#f00', purple: '#f0f', cyan: '#00ffe4'},
	},

	/**
	 * Our main Territories object
	 * It looks like:
	 * Territories: {
	 *     Alaska: {path: Object, color: String, name: 'Alaska', ...},
	 *	   ... 
	 *	}
	 */
	Territories: {},

	stage: null,
	mapLayer: null,
	topLayer:  null,
	backgroundLayer: null,

	init: function() {
		//Initiate our main Territories Object, it contains essential data about the territories current state
		Risk.setUpTerritoriesObj();

		//Initiate a Kinetic stage
		Risk.stage = new Kinetic.Stage({
			container: 'map',
			width: 1920,
			height: 1080
		});

		Risk.mapLayer = new Kinetic.Layer({
			scale: Risk.Settings.globalScale
		});

		Risk.topLayer = new Kinetic.Layer({
			scale: Risk.Settings.globalScale
		});

		Risk.drawBackgroundImg();
		Risk.drawTerritories();

		Risk.stage.add(Risk.backgroundLayer);
		Risk.stage.add(Risk.mapLayer);
		Risk.stage.add(Risk.topLayer);

		Risk.mapLayer.draw();

		Risk.divideTerritories();
	},

	/**
	 * Initiate the  Risk.Territories Object, this will contain essential informations about the territories 
	 */
	setUpTerritoriesObj: function() {
		for(id in TerritoryNames) {

			var pathObject = new Kinetic.Path({
				data: TerritoryPathData[id].path,
				id: id //set a unique id --> path.attrs.id
			});

			//Using a sprite image for territory names
			//see: drawImage() -- https://developer.mozilla.org/en-US/docs/Canvas_tutorial/Using_images , and see Kinetic.Image() docs for more
			var sprite = new Image();
			sprite.src = 'img/names.png';
			var territoryNameImg = new Kinetic.Image({
				image: sprite,
				x: FontDestinationCoords[id].x,
				y: FontDestinationCoords[id].y,
				width: FontSpriteCoords[id].sWidth, //'destiantion Width' 
				height: FontSpriteCoords[id].sHeight, //'destination Height'
				crop: [FontSpriteCoords[id].sx, FontSpriteCoords[id].sy, FontSpriteCoords[id].sWidth, FontSpriteCoords[id].sHeight]

			});

			Risk.Territories[id] = {
				name: TerritoryNames[id],
				path: pathObject,
				nameImg: territoryNameImg,
				color: null,
				neighbours: Neighbours[id],
				armyNum: null
			};
		}
		
	},

	drawBackgroundImg: function() {
		Risk.backgroundLayer = new Kinetic.Layer({
			scale: Risk.Settings.globalScale
		});
		var imgObj = new Image();
		imgObj.src = 'img/map_grey.jpg';
		
		var img = new Kinetic.Image({
			image: imgObj,
			//alpha: 0.8
		});
		Risk.backgroundLayer.add(img);
	},

	drawTerritories: function() {
		for (t in Risk.Territories) {
			
			var path = Risk.Territories[t].path;
			var nameImg = Risk.Territories[t].nameImg;
			var group = new Kinetic.Group();

			//We have to set up a group for proper mouseover on territories and sprite name images 
			group.add(path);
			group.add(nameImg);
			Risk.mapLayer.add(group);
		
			//Basic animations 
			//Wrap the 'path', 't' and 'group' variables inside a closure, and set up the mouseover / mouseout events for the demo
			//when you make a bigger application you should move this functionality out from here, and maybe put these 'actions' in a seperate function/'class'
			(function(path, t, group) {
				group.on('mouseover', function() {
					path.setFill('#eee');
					path.setOpacity(0.3);
					group.moveTo(Risk.topLayer);
					Risk.topLayer.drawScene();
				});

				group.on('mouseout', function() {
					path.setFill(Risk.Settings.colors[Risk.Territories[t].color]);
					path.setOpacity(0.4);
					group.moveTo(Risk.mapLayer);
					Risk.topLayer.draw();
				});

				group.on('click', function() {
					console.log(path.attrs.id);
					location.hash = path.attrs.id;
				});
			})(path, t, group);
		}				
	},

	divideTerritories: function() {

		fillRandomColors();

		for(var id in Risk.Territories) {
			var color = Risk.Territories[id].color;
			
			var neighbours = Risk.Territories[id].neighbours;

			//a VERY simple algorithm to make the map more equal
			var similarNeighbours = 0;
			for(var i = 0; i < neighbours.length; i++) {

				var currNeighbour = neighbours[i];
				if (Risk.Territories[currNeighbour].color == color) {
					similarNeighbours++;
				}
			}

			//how many similar neighbours we allow
			if (similarNeighbours > 2) {
				var newColor = getRandomColor();
				while (color == newColor) {
					var newColor = getRandomColor();
				}
				Risk.Territories[id].color = newColor;

				Risk.Territories[id].path.setFill(Risk.Settings.colors[newColor]);
				Risk.Territories[id].path.setOpacity(0.4);				
			}
		}

		Risk.mapLayer.draw();

		function fillRandomColors() {
			for(var id in Risk.Territories) {
				var color = getRandomColor();
				Risk.Territories[id].color = color;
				Risk.Territories[id].path.setFill(Risk.Settings.colors[color]);
				Risk.Territories[id].path.setOpacity(0.4);			

			}
		}

		/**
		 * Returns a color name like 'yellow'
		 */
		function getRandomColor() {
			var colors = ['yellow', 'green', 'blue'];
			//Math.random() returns between [0, 1), so don't worry
			var randomNum = Math.floor(Math.random()*(colors.length)); 
			return colors[randomNum];
		}
	}
}
