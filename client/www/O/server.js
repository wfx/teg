var server=require( 'node-http-server' );

var msg = function ( msg ) {
    console.log ( msg );
}

server.deploy(
    {
        port:8000,
        root:'../'
    }
);
msg( 'teg server running on: 127.0.0.1:8000' );

//  notion a way to go.

/*  Assign the countries to the players at the beginning of the game.
    #1, roll the dice for player play sequence.
    #2, get random country that is not yet owned by anyone
    #4, get real player number via reference and
    #5, assing the chosen country to this player
    #6, give the country one army

    Game turn's.
    #1, get armies
    #2, set armies
    #3, ~atack
    #4, ~get gards
    #5, end turn
*/

// Store all state: save game, player lsiot connection, rejoin...
// Using SQLite (npm install sqlite3) ?
PLAYER_STATE = [ 'LOST_GAME', 'IN_GAME',
                 'GET_ARMY', 'SET_ARMY',
                 'ATACK', 'DEFEND',
                 'GET_CARD','CHANGE_CARD',
                 'CONQUERED_COUNTRY', 'LOST_COUNTRY'
               ];

GAME_STAE = [];

/*  id        : int 1-6
    name      : str
    state     : int
*/
var players = new Object(),
    id        = 0,
    name      = '',
    state     = 0,
    countries = [ id ],
    cards     = [ id ],
    mission   = 0;

var countries_assign = new Object();

