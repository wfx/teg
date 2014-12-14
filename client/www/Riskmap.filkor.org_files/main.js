/*
We have to use an asset manager (located in assetManager.js) to download every large picture before we show anything, 
otherwise the map would be imperfect (some images may not appear or load slowly)
 */

var ASSET_MANAGER = new AssetManager();

ASSET_MANAGER.queueDownload('img/map_grey.jpg');
ASSET_MANAGER.queueDownload('img/names.png');

ASSET_MANAGER.downloadAll(function() {
	Risk.init();
	$("#overlay").fadeOut('slow');
});