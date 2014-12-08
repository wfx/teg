var loc = window.location.pathname;
//var basedir = loc.substring(0, loc.lastIndexOf('/www'));
var tboard = new CDrawBoard("Board","m2");

var CBoard = function(){
    this.name    = null;
    this.image   = new Image();
    this.width   = 0;
    this.height  = 0;
};

var CContinent = function(){
    this.name    = null;
    this.country = [];
    this.image   = new Image();
    this.pos_x   = 0;
    this.pos_y   = 0;
};

var CCountry = function(){
    this.name    = null;
    this.image   = new Image();
    this.pos_x   = 0;
    this.pos_y   = 0;
    this.border  = [];       // countries
    this.owner   = null;     // player
    this.army    = null;     // amount
        
    this.addArmy = function(n) {
        army = army + n;
    };
    
    this.subArmy = function(n) {
        if (army - n >= 0) {
            army = army - n;
        } else {
            army = 0;
        }
    };
    
    this.setOwner = function(id) {
        owner = id;
    };
};

function CDrawBoard(canvas,theme){
    var TCanvas     = document.getElementById(canvas);
    var TContext    = TCanvas.getContext("2d");
    var TContinent  = [];
    var TCountry    = [];
    var TTheme      = "themes/" + theme + "/teg_theme.xml";
    var fileRoot    = "themes/" + theme + "/";
    $.ajax({
        type: "GET",
        url: TTheme,
        dataType: "xml",
        success: function(theme){
            var n = 0;
            $(theme).find('board').each(function(){
                TBoard = new CBoard();
                TBoard.image.src = fileRoot + $(this).attr('file');
                TBoard.width = $(this).attr('width');
                TBoard.height = $(this).attr('height');
            });
            $(theme).find('continent').each(function(i){
                TContinent[i] = new CContinent();
                TContinent[i].name = $(this).attr('name');
                TContinent[i].pos_x = $(this).attr('pos_x');
                TContinent[i].pos_y = $(this).attr('pos_y');
                $(this).find('country').each(function(){
                    TCountry[n] = new CCountry();
                    TCountry[n].name  = $(this).attr('name');
                    TCountry[n].image.src = fileRoot + $(this).attr('file');
                    TCountry[n].pos_x = parseInt(TContinent[i].pos_x) + parseInt($(this).attr('pos_x'));
                    TCountry[n].pos_y = parseInt(TContinent[i].pos_y) + parseInt($(this).attr('pos_y'));
                    TContinent[i].country[n] = TCountry[n].name;
                    n++;
                });
                Draw(TBoard, TContinent[i],TCountry,TContext);
            });
        },
        error: function(theme){
            console.log("error : " + TTheme);
        }
    });

    function Draw(Board,Continent,Country,Context){
        Context.drawImage(Board.image, 0, 0);
        $.each(Continent.country, function(n){
            Country[n].image.onload = function(){
                Context.drawImage(Country[n].image, Country[n].pos_x, Country[n].pos_y);
            };
        });
    };
}