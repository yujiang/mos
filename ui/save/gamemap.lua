gamemap = {
   ["childs"] = {
      ["1"] = {
         ["childs"] = {
            ["bg"] = {
               ["ani"] = {
                  ["timer"] = {
                     ["clock_time"] = 43.823;
                     ["type"] = 1;
                     ["time"] = 0.25;
                     ["func"] = "function: 005EADB8, defined in (35-55)@.\\game\\ani.lua";
                     ["param"] = {}; -- gamemap["childs"]["1"]["childs"]["bg"]["ani"] (self reference)
                  };
                  ["image"] = {}; -- gamemap["childs"]["1"]["childs"]["bg"] (self reference)
                  ["frame_end"] = 3;
                  ["step"] = 1;
                  ["speed"] = 0.25;
                  ["loop"] = true;
                  ["frame_start"] = 1;
                  ["class_name"] = "ani";
               };
               ["childs"] = {};
               ["_disable"] = true;
               ["w"] = 105;
               ["y"] = -80;
               ["h"] = 95;
               ["name"] = "bg";
               ["z"] = -1000;
               ["class_name"] = "image";
               ["frame"] = 2;
               ["x"] = -50;
               ["image_file"] = "dog";
            };
            ["caption"] = {
               ["childs"] = {
                  ["text"] = {
                     ["childs"] = {};
                     ["color"] = 65280;
                     ["class_name"] = "text";
                     ["line"] = 1;
                     ["text_w"] = 32;
                     ["align"] = 1;
                     ["wrap"] = 0;
                     ["w"] = 105;
                     ["text_h"] = 16;
                     ["y"] = 0;
                     ["x"] = 0;
                     ["name"] = "text";
                     ["z"] = 0;
                     ["text"] = "精灵";
                     ["h"] = 95;
                     ["font"] = 0;
                  };
               };
               ["_disable"] = true;
               ["w"] = 32;
               ["y"] = 0;
               ["h"] = 16;
               ["name"] = "caption";
               ["z"] = 1000;
               ["x"] = -16;
               ["class_name"] = "label";
            };
         };
         ["ani_id"] = 101;
         ["w"] = 105;
         ["y"] = 79.662523903264;
         ["h"] = 95;
         ["name"] = "1";
         ["z"] = 0;
         ["move"] = {
            ["x_start"] = 100;
            ["y_start"] = 100;
            ["y_speed"] = -14.834045293024;
            ["y_des"] = 76;
            ["on_reached"] = "function: 00640F60, defined in (59-62)@.\\game\\sprite.lua";
            ["x_speed"] = 98.89363528683;
            ["x_des"] = 260;
            ["timer"] = {
               ["func"] = "function: 005A1FB8, defined in (22-38)@.\\game\\move.lua";
               ["type"] = 4;
               ["time"] = 0;
               ["param"] = {}; -- gamemap["childs"]["1"]["move"] (self reference)
            };
            ["cell"] = {}; -- gamemap["childs"]["1"] (self reference)
            ["class_name"] = "move";
         };
         ["x"] = 235.58317397824;
         ["class_name"] = "sprite";
      };
      ["3"] = {
         ["ai"] = {
            ["class_name"] = "ai";
            ["sprite"] = {}; -- gamemap["childs"]["3"] (self reference)
         };
         ["childs"] = {
            ["bg"] = {
               ["ani"] = {
                  ["timer"] = {
                     ["clock_time"] = 43.744;
                     ["type"] = 1;
                     ["time"] = 0.25;
                     ["func"] = "function: 005EADB8, defined in (35-55)@.\\game\\ani.lua";
                     ["param"] = {}; -- gamemap["childs"]["3"]["childs"]["bg"]["ani"] (self reference)
                  };
                  ["image"] = {}; -- gamemap["childs"]["3"]["childs"]["bg"] (self reference)
                  ["frame_end"] = 3;
                  ["step"] = 1;
                  ["speed"] = 0.25;
                  ["loop"] = true;
                  ["frame_start"] = 1;
                  ["class_name"] = "ani";
               };
               ["childs"] = {};
               ["_disable"] = true;
               ["w"] = 105;
               ["y"] = -80;
               ["h"] = 95;
               ["name"] = "bg";
               ["z"] = -1000;
               ["class_name"] = "image";
               ["frame"] = 2;
               ["x"] = -50;
               ["image_file"] = "dog";
            };
            ["caption"] = {
               ["childs"] = {
                  ["text"] = {
                     ["childs"] = {};
                     ["color"] = 9455392;
                     ["class_name"] = "text";
                     ["line"] = 1;
                     ["text_w"] = 64;
                     ["align"] = 1;
                     ["wrap"] = 0;
                     ["w"] = 105;
                     ["text_h"] = 16;
                     ["y"] = 0;
                     ["x"] = 0;
                     ["name"] = "text";
                     ["z"] = 0;
                     ["text"] = "sprite_3";
                     ["h"] = 95;
                     ["font"] = 0;
                  };
               };
               ["_disable"] = true;
               ["w"] = 64;
               ["y"] = 0;
               ["h"] = 16;
               ["name"] = "caption";
               ["z"] = 1000;
               ["x"] = -32;
               ["class_name"] = "label";
            };
         };
         ["move"] = {
            ["x_start"] = 48;
            ["y_start"] = 525;
            ["y_speed"] = -64.353502836381;
            ["y_des"] = 393;
            ["on_reached"] = "function: 0066F768, defined in (59-62)@.\\game\\sprite.lua";
            ["x_speed"] = 76.541666252362;
            ["x_des"] = 205;
            ["timer"] = {
               ["func"] = "function: 005A1FB8, defined in (22-38)@.\\game\\move.lua";
               ["type"] = 4;
               ["time"] = 0;
               ["param"] = {}; -- gamemap["childs"]["3"]["move"] (self reference)
            };
            ["cell"] = {}; -- gamemap["childs"]["3"] (self reference)
            ["class_name"] = "move";
         };
         ["ani_id"] = 101;
         ["w"] = 105;
         ["on_reached"] = "function: 0066FB78, defined in (48-51)@.\\game\\ai.lua";
         ["y"] = 431.94483489859;
         ["h"] = 95;
         ["name"] = "3";
         ["z"] = 0;
         ["is_hover"] = false;
         ["x"] = 158.67924940092;
         ["class_name"] = "sprite";
      };
      ["bg"] = {
         ["childs"] = {};
         ["_disable"] = true;
         ["w"] = 800;
         ["y"] = 0;
         ["h"] = 800;
         ["name"] = "bg";
         ["z"] = -1000;
         ["class_name"] = "image";
         ["frame"] = 0;
         ["x"] = 0;
         ["image_file"] = "farm.jpg";
      };
      ["2"] = {
         ["ai"] = {
            ["class_name"] = "ai";
            ["sprite"] = {}; -- gamemap["childs"]["2"] (self reference)
         };
         ["childs"] = {
            ["bg"] = {
               ["ani"] = {
                  ["timer"] = {
                     ["clock_time"] = 43.64;
                     ["type"] = 1;
                     ["time"] = 0.25;
                     ["func"] = "function: 005EADB8, defined in (35-55)@.\\game\\ani.lua";
                     ["param"] = {}; -- gamemap["childs"]["2"]["childs"]["bg"]["ani"] (self reference)
                  };
                  ["image"] = {}; -- gamemap["childs"]["2"]["childs"]["bg"] (self reference)
                  ["frame_end"] = 3;
                  ["step"] = 1;
                  ["speed"] = 0.25;
                  ["loop"] = true;
                  ["frame_start"] = 1;
                  ["class_name"] = "ani";
               };
               ["childs"] = {};
               ["_disable"] = true;
               ["w"] = 105;
               ["y"] = -80;
               ["h"] = 95;
               ["name"] = "bg";
               ["z"] = -1000;
               ["class_name"] = "image";
               ["frame"] = 2;
               ["x"] = -50;
               ["image_file"] = "dog";
            };
            ["caption"] = {
               ["childs"] = {
                  ["text"] = {
                     ["childs"] = {};
                     ["color"] = 20993;
                     ["class_name"] = "text";
                     ["line"] = 1;
                     ["text_w"] = 64;
                     ["align"] = 1;
                     ["wrap"] = 0;
                     ["w"] = 105;
                     ["text_h"] = 16;
                     ["y"] = 0;
                     ["x"] = 0;
                     ["name"] = "text";
                     ["z"] = 0;
                     ["text"] = "sprite_2";
                     ["h"] = 95;
                     ["font"] = 0;
                  };
               };
               ["_disable"] = true;
               ["w"] = 64;
               ["y"] = 0;
               ["h"] = 16;
               ["name"] = "caption";
               ["z"] = 1000;
               ["x"] = -32;
               ["class_name"] = "label";
            };
         };
         ["move"] = {
            ["x_start"] = 100;
            ["y_start"] = 100;
            ["y_speed"] = 0;
            ["y_des"] = 100;
            ["on_reached"] = "function: 0106F5A8, defined in (59-62)@.\\game\\sprite.lua";
            ["x_speed"] = 100;
            ["x_des"] = 600;
            ["timer"] = {
               ["func"] = "function: 005A1FB8, defined in (22-38)@.\\game\\move.lua";
               ["type"] = 4;
               ["time"] = 0;
               ["param"] = {}; -- gamemap["childs"]["2"]["move"] (self reference)
            };
            ["cell"] = {}; -- gamemap["childs"]["2"] (self reference)
            ["class_name"] = "move";
         };
         ["ani_id"] = 101;
         ["w"] = 105;
         ["on_reached"] = "function: 0106F958, defined in (24-27)@.\\game\\ai.lua";
         ["y"] = 100;
         ["h"] = 95;
         ["name"] = "2";
         ["z"] = 0;
         ["is_hover"] = false;
         ["x"] = 520.6;
         ["class_name"] = "sprite";
      };
   };
   ["is_window"] = true;
   ["layer"] = "gamemap";
   ["drag"] = false;
   ["w"] = 800;
   ["is_hover"] = false;
   ["y"] = 0;
   ["h"] = 800;
   ["name"] = "gamemap";
   ["z"] = 0;
   ["is_focus"] = false;
   ["x"] = 0;
   ["class_name"] = "map";
};
gamemap["childs"]["1"]["childs"]["bg"]["ani"]["timer"]["param"] = gamemap["childs"]["1"]["childs"]["bg"]["ani"];
gamemap["childs"]["1"]["childs"]["bg"]["ani"]["image"] = gamemap["childs"]["1"]["childs"]["bg"];
gamemap["childs"]["1"]["move"]["timer"]["param"] = gamemap["childs"]["1"]["move"];
gamemap["childs"]["1"]["move"]["cell"] = gamemap["childs"]["1"];
gamemap["childs"]["3"]["ai"]["sprite"] = gamemap["childs"]["3"];
gamemap["childs"]["3"]["childs"]["bg"]["ani"]["timer"]["param"] = gamemap["childs"]["3"]["childs"]["bg"]["ani"];
gamemap["childs"]["3"]["childs"]["bg"]["ani"]["image"] = gamemap["childs"]["3"]["childs"]["bg"];
gamemap["childs"]["3"]["move"]["timer"]["param"] = gamemap["childs"]["3"]["move"];
gamemap["childs"]["3"]["move"]["cell"] = gamemap["childs"]["3"];
gamemap["childs"]["2"]["ai"]["sprite"] = gamemap["childs"]["2"];
gamemap["childs"]["2"]["childs"]["bg"]["ani"]["timer"]["param"] = gamemap["childs"]["2"]["childs"]["bg"]["ani"];
gamemap["childs"]["2"]["childs"]["bg"]["ani"]["image"] = gamemap["childs"]["2"]["childs"]["bg"];
gamemap["childs"]["2"]["move"]["timer"]["param"] = gamemap["childs"]["2"]["move"];
gamemap["childs"]["2"]["move"]["cell"] = gamemap["childs"]["2"];

return gamemap