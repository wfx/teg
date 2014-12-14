/**
 * Basic structure that maps the id's to territory names. 
 * We use this structure to build the Risk.Territories Object
 * @type {Object}
 */
var TerritoryNames = {	
	Alaska: 'Alaska',
	NorthWestTerritory: 'North West Territory',
	Alberta: 'Alberta',
	Ontario: 'Ontario',
	Greenland: 'Greenland',
	Quebec: 'Quebec',
	WesternUnitedStates: 'Western United States',
	EasternUnitedStates: 'Eastern United States',
	CentralAmerica: 'Central America',
	Peru: 'Peru',
	Brazil: 'Brazil',
	Venezuela: 'Venezuela',
	Argentina: 'Argentina',
	NorthAfrica: 'North Africa',
	Egypt: 'Egypt',
	Congo: 'Congo',
	EastAfrica: 'East Africa',
	SouthAfrica: 'South Africa',
	Iceland: 'Iceland',
	GreatBritain: 'Great Britain',
	WesternEurope: 'Western Europe',
	NorthernEurope: 'Northern Europe',
	SouthernEurope: 'Southern Europe',
	Scandinavia: 'Scandinavia',
	Madagascar: 'Madagascar',
	Ukraine: 'Ukraine',
	MiddleEast: 'Middle East',
	Afghanistan: 'Afghanistan',
	Ural: 'Ural',
	India: 'India',
	Siam: 'Siam',
	China: 'China',
	Mongolia: 'Mongolia',
	Irkutsk: 'Irkutsk',
	Yakutsk: 'Yakutsk',
	Siberia: 'Siberia',
	Kamchatka: 'Kamchatka',
	Japan: 'Japan',
	Indonesia: 'Indonesia',
	NewGuinea: 'New Guinea',
	WesternAustralia: 'Western Australia',
	EasternAustralia: 'Eastern Australia'
};


/**
 * Containts the neighbour data for the specific territories
 * @type {Object}
 */
var Neighbours = {
	Alaska: ['NorthWestTerritory','Alberta', 'Kamchatka'],
	NorthWestTerritory: ['Alberta','Alaska','Greenland'],
	Alberta: ['NorthWestTerritory','Alaska','Ontario','WesternUnitedStates'],
	Ontario: ['NorthWestTerritory','Alberta','WesternUnitedStates','EasternUnitedStates','Quebec','Greenland'],
	Greenland: ['NorthWestTerritory','Ontario','Quebec','Iceland'], 
	Quebec: ['Ontario','EasternUnitedStates','Greenland'],
	WesternUnitedStates: ['Alberta','Ontario','EasternUnitedStates','CentralAmerica'],
	EasternUnitedStates: ['Quebec','Ontario','WesternUnitedStates','CentralAmerica'],
	CentralAmerica: ['WesternUnitedStates','EasternUnitedStates','Venezuela'],
	Peru: ['Venezuela','Brazil','Argentina'],
	Brazil: ['Venezuela','Peru','Argentina','NorthAfrica'],
	Venezuela: ['Peru','Brazil','CentralAmerica'],
	Argentina: ['Peru','Brazil'],
	NorthAfrica: ['Brazil','Congo','EastAfrica','Egypt','SouthernEurope','WesternEurope'],
	Egypt: ['NorthAfrica','EastAfrica','MiddleEast','SouthernEurope'],
	Congo: ['NorthAfrica','EastAfrica','SouthAfrica'],
	EastAfrica: ['Egypt','NorthAfrica','Congo','SouthAfrica','Madagascar','MiddleEast'],
	SouthAfrica: ['Congo','EastAfrica','Madagascar'],
	Iceland: ['Greenland','GreatBritain','Scandinavia'],
	GreatBritain: ['Iceland','Scandinavia','NorthernEurope','WesternEurope'],
	WesternEurope: ['GreatBritain','NorthernEurope','SouthernEurope','NorthAfrica'],
	NorthernEurope: ['Scandinavia','GreatBritain','WesternEurope','SouthernEurope','Ukraine'],
	SouthernEurope: ['Ukraine','NorthernEurope','WesternEurope','NorthAfrica','Egypt','MiddleEast'],
	Scandinavia: ['Iceland', 'GreatBritain', 'NorthernEurope', 'Ukraine'],
	Madagascar: ['SouthAfrica','EastAfrica'], 
	Ukraine: ['Scandinavia','NorthernEurope','SouthernEurope','MiddleEast','Afghanistan','Ural'],
	MiddleEast: ['Ukraine','SouthernEurope','Egypt','EastAfrica','India','Afghanistan'],
	Afghanistan: ['Ural','Ukraine','MiddleEast','India','China'],
	Ural: ['Ukraine','Afghanistan','China','Siberia'],
	India: ['MiddleEast','Afghanistan','China','Siam'],
	Siam: ['Indonesia','China','India'],
	China: ['Mongolia','Siberia','Ural','Afghanistan','India','Siam'],
	Mongolia: ['Japan','Kamchatka','Irkutsk','Siberia','China'],
	Irkutsk: ['Kamchatka','Yakutsk','Siberia','Mongolia'],
	Yakutsk: ['Kamchatka','Irkutsk','Siberia'],
	Siberia: ['Ural','China','Mongolia','Irkutsk','Yakutsk'],
	Kamchatka: ['Alaska','Japan','Mongolia','Irkutsk','Yakutsk'],
	Japan: ['Kamchatka','Mongolia'],
	Indonesia: ['Siam','NewGuinea','WesternAustralia'],
	NewGuinea: ['Indonesia','WesternAustralia','EasternAustralia'],
	WesternAustralia: ['Indonesia','NewGuinea','EasternAustralia'],
	EasternAustralia: ['NewGuinea','WesternAustralia'],
}


var CenterPoints = {
	Alaska: {x: 86, y: 159},
	NorthWestTerritory: {x: 228, y: 153}, 
	Alberta: {x: 223, y: 247},
	Ontario: {x: 323, y: 271}, 
	Greenland: {x: 543, y: 88}, 
	Quebec: {x: 439, y: 268}, 
	WesternUnitedStates: {x: 233, y: 369}, 
	EasternUnitedStates: {x: 351, y: 405},
	CentralAmerica: {x: 250, y: 521},
	Peru: {x: 377, y: 766},
	Brazil: {x: 485, y: 725},
	Venezuela: {x: 352, y: 615},
	Argentina: {x: 402, y: 895},
	NorthAfrica: {x: 733, y: 666},
	Egypt: {x: 863, y: 634},
	Congo: {x: 864, y: 823},	
	EastAfrica: {x: 932, y: 738},
	SouthAfrica: {x: 873, y: 959},
	Iceland: {x: 680, y: 213},
	GreatBritain: {x: 653, y: 342},
	WesternEurope: {x: 680, y: 497},
	NorthernEurope: {x: 795, y: 359},
	SouthernEurope: {x: 804, y: 455},
	Scandinavia: {x: 799, y: 200},
	Madagascar: {x: 1025, y: 967},
	Ukraine: {x: 953, y: 281},
	MiddleEast: {x: 981, y: 576}, 
	Afghanistan: {x: 1087, y: 401},
	Ural: {x: 1119, y: 244},
	India: {x: 1182, y: 581},
	Siam: {x: 1309, y: 603},
	China: {x: 1276, y: 476},
	Mongolia: {x: 1324, y: 364},
	Irkutsk: {x: 1310, y: 270},
	Yakutsk: {x: 1323, y: 133},
	Siberia: {x: 1204, y: 162},
	Kamchatka: {x: 1460, y: 138},
	Japan: {x: 1500, y: 380}, 
	Indonesia: {x: 1338, y: 799},
	NewGuinea: {x: 1480, y: 754},
	WesternAustralia: {x: 1404, y: 959},
	EasternAustralia: {x: 1546, y: 957},
};

var ArmyPoints = {
	Alaska: {x: 86, y: 130},
	NorthWestTerritory: {x: 232, y: 120},
	Alberta: {x: 218, y: 223},
	Ontario: {x: 339, y: 242},
	Greenland: {x: 539, y: 134},
	Quebec: {x: 423, y: 308},
	WesternUnitedStates: {x: 184, y: 399}, 
	EasternUnitedStates: {x: 407, y: 392},
	CentralAmerica: {x: 292, y: 527},
	Peru: {x: 339, y: 756}, 
	Brazil: {x: 512, y: 752}, 
	Venezuela: {x: 384, y: 583}, 
	Argentina: {x: 378, y: 922}, 
	NorthAfrica: {x: 730, y: 655},
	Egypt: {x: 859, y: 604},
	Congo: {x: 865, y: 853},
	EastAfrica: {x: 923, y: 719},
	SouthAfrica: {x: 858, y: 916},
	Iceland: {x: 688, y: 178},
	GreatBritain: {x: 647, y: 305},
	WesternEurope: {x: 630, y: 490}, 
	NorthernEurope: {x: 798, y: 316},
	SouthernEurope: {x: 808, y: 493}, 
	Scandinavia: {x: 820, y: 231},
	Madagascar: {x: 1054, y: 977}, 
	Ukraine: {x: 944, y: 306}, 
	MiddleEast: {x: 1005, y: 583}, 
	Afghanistan: {x: 1076, y: 427},
	Ural: {x: 1117, y: 287},
	India: {x: 1178, y: 592}, 
	Siam: {x: 1354, y: 641},
	China: {x: 1313, y: 510}, 
	Mongolia: {x: 1330, y: 402},
	Irkutsk: {x: 1282, y: 285}, 
	Yakutsk: {x: 1330, y: 84}, 
	Siberia: {x: 1189, y: 123}, 
	Kamchatka: {x: 1441, y: 168},
	Japan: {x: 1506, y: 349}, 
	Indonesia: {x: 1337, y: 763},
	NewGuinea: {x: 1473, y: 707}, 
	WesternAustralia: {x: 1384, y: 920}, 
	EasternAustralia: {x: 1546, y: 957}
}


/*
temp {
	Alaska: 
	NorthWestTerritory: 
	Alberta: 
	Ontario: 
	Greenland: 
	Quebec: 
	WesternUnitedStates: 
	EasternUnitedStates: 
	CentralAmerica: 
	Peru: 
	Brazil: 
	Venezuela: 
	Argentina: 
	NorthAfrica: 
	Egypt: 
	Congo: 
	EastAfrica: 
	SouthAfrica: 
	Iceland: 
	GreatBritain: 
	WesternEurope: 
	NorthernEurope: 
	SouthernEurope: 
	Scandinavia: 
	Madagascar: 
	Ukraine: 
	MiddleEast: 
	Afghanistan: 
	Ural: 
	India: 
	Siam: 
	China: 
	Mongolia: 
	Irkutsk: 
	Yakutsk: 
	Siberia: 
	Kamchatka: 
	Japan: 
	Indonesia: 
	NewGuinea: 
	WesternAustralia: 
	EasternAustralia: 
}
 */