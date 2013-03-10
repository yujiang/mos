win3 = {
   ["childs"] = {
      ["bg"] = {
         ["childs"] = {};
         ["_disable"] = true;
         ["alpha"] = 200;
         ["w"] = 182;
         ["y"] = 0;
         ["h"] = 94;
         ["name"] = "bg";
         ["z"] = -1000;
         ["class_name"] = "image";
         ["frame"] = 0;
         ["x"] = 0;
         ["image_file"] = "land.png";
      };
      ["ani_dog"] = {
         ["ani"] = {
            ["timer"] = {
               ["clock_time"] = 293.57;
               ["time"] = 0.5;
               ["func"] = "function: 01657008, defined in (36-56)@.\\game\\ani.lua";
               ["param"] = {}; -- win3["childs"]["ani_dog"]["ani"] (self reference)
            };
            ["image"] = {}; -- win3["childs"]["ani_dog"] (self reference)
            ["frame_to"] = 3;
            ["step"] = 1;
            ["speed"] = 0.5;
            ["loop"] = true;
            ["frame_start"] = 1;
            ["class_name"] = "ani";
         };
         ["childs"] = {};
         ["_disable"] = true;
         ["w"] = 105;
         ["y"] = -40;
         ["h"] = 95;
         ["name"] = "ani_dog";
         ["z"] = 0;
         ["class_name"] = "image";
         ["frame"] = 1;
         ["x"] = 40;
         ["image_file"] = "dog";
      };
   };
   ["x"] = 185;
   ["is_window"] = true;
   ["drag"] = true;
   ["w"] = 182;
   ["is_hover"] = false;
   ["y"] = 103;
   ["h"] = 94;
   ["name"] = "win3";
   ["z"] = 36;
   ["is_focus"] = false;
   ["is_drag"] = false;
   ["class_name"] = "window";
};
win3["childs"]["ani_dog"]["ani"]["timer"]["param"] = win3["childs"]["ani_dog"]["ani"];
win3["childs"]["ani_dog"]["ani"]["image"] = win3["childs"]["ani_dog"];

return win3