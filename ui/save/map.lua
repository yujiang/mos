local map = {
   ["layer"] = "map";
   ["w"] = 4000;
   ["childs"] = {
      ["bg"] = {
         ["childs"] = {};
         ["_disable"] = true;
         ["w"] = 4000;
         ["room"] = 1;
         ["y"] = 0;
         ["h"] = 2960;
         ["name"] = "bg";
         ["z"] = -1000;
         ["map_file"] = "map/1005.map";
         ["frame"] = 0;
         ["x"] = 0;
         ["class_name"] = "image_map";
      };
      ["1"] = {
         ["childs"] = {
            ["caption"] = {
               ["childs"] = {
                  ["text"] = {
                     ["childs"] = {};
                     ["color"] = 65280;
                     ["class_name"] = "text";
                     ["h"] = 32;
                     ["text_w"] = 32;
                     ["align"] = 1;
                     ["wrap"] = 0;
                     ["w"] = 100;
                     ["text_h"] = 16;
                     ["y"] = 0;
                     ["x"] = 0;
                     ["name"] = "text";
                     ["z"] = 0;
                     ["line"] = 1;
                     ["text"] = "猫猫";
                     ["font"] = 0;
                     ["room"] = 1;
                  };
               };
               ["_disable"] = true;
               ["w"] = 32;
               ["y"] = 0;
               ["h"] = 16;
               ["name"] = "caption";
               ["z"] = 1000;
               ["room"] = 1;
               ["x"] = -16;
               ["class_name"] = "label";
            };
            ["body"] = {
               ["ani"] = {
                  ["on_aniend"] = "function: 07A19CA8, defined in (117-121)@.\\game\\sprite_body.lua";
                  ["frame_end"] = 64;
                  ["ani_tb"] = {
                     ["ani_speed"] = 0.3;
                     ["frame_end"] = 8;
                     ["dir"] = 8;
                     ["frame_start"] = 0;
                     ["frame_step"] = 1;
                     ["name"] = "stand";
                     ["dir_inverse"] = true;
                     ["loop"] = false;
                     ["dir_frame"] = 8;
                     ["shape"] = 120;
                     ["image_file"] = "char/0120/stand.zgp";
                  };
                  ["image"] = {}; -- map["childs"]["1"]["childs"]["body"] (self reference)
                  ["ani_dir"] = 2;
                  ["step"] = 1;
                  ["speed"] = 0.3;
                  ["loop"] = false;
                  ["frame_start"] = 56;
                  ["class_name"] = "ani";
               };
               ["childs"] = {
                  ["weapon"] = {
                     ["cy"] = 79;
                     ["cx"] = 7;
                     ["childs"] = {};
                     ["room"] = 1;
                     ["class_name"] = "image";
                     ["weapon_id"] = 2;
                     ["w"] = 7;
                     ["y"] = 0;
                     ["h"] = 72;
                     ["name"] = "weapon";
                     ["z"] = 1;
                     ["need_mask"] = true;
                     ["frame"] = 56;
                     ["x"] = 0;
                     ["image_file"] = "char/0120/02/stand.zgp";
                  };
               };
               ["_disable"] = true;
               ["ani_id"] = 120;
               ["class_name"] = "sprite_body";
               ["part4"] = 3840276;
               ["h"] = 92;
               ["cy"] = 87;
               ["cx"] = 53;
               ["ani_name"] = "stand";
               ["part1"] = 1693972;
               ["need_mask"] = true;
               ["w"] = 71;
               ["image_file"] = "char/0120/stand.zgp";
               ["y"] = 0;
               ["x"] = 0;
               ["name"] = "body";
               ["z"] = -1000;
               ["frame"] = 56;
               ["room"] = 1;
               ["use_zgp"] = true;
            };
         };
         ["emitter"] = {
            ["class_name"] = "emitter";
         };
         ["w"] = 0;
         ["y"] = 789;
         ["h"] = 0;
         ["name"] = "1";
         ["z"] = 0;
         ["room"] = 1;
         ["x"] = 1734;
         ["class_name"] = "sprite";
      };
   };
   ["is_hover"] = false;
   ["drag"] = false;
   ["class_name"] = "map";
   ["is_map"] = true;
   ["y"] = -489;
   ["room"] = 1;
   ["name"] = "map";
   ["z"] = 0;
   ["is_focus"] = false;
   ["x"] = -1334;
   ["h"] = 2960;
};
map["childs"]["1"]["childs"]["body"]["ani"]["image"] = map["childs"]["1"]["childs"]["body"];

return map