# t3d
T3D is a simple "tiled" 3d level editor, mostly aimed for speed world building.
Inspirations are
* tiled (obviosly): https://www.mapeditor.org/
* unity snaps system: https://www.youtube.com/watch?v=b4oqOdBCy3c
* sketchup
* the tony hawk pro skater 2 - create a park: https://www.youtube.com/watch?v=DWGNN2ACJao
* starcraft level editor
* portal 2 level editor: https://www.youtube.com/watch?v=iJgllufikSo
* bethesda level editor: [Fallout 4's Modular Level Design](https://www.youtube.com/watch?v=QBAM27YbKZg) / [Skyrim’s Modular Approach to Level Design](http://blog.joelburgess.com/2013/04/skyrims-modular-level-design-gdc-2013.html)
* asset forge [demo video](https://www.youtube.com/watch?v=tXUyR2D8Pxc)
* asset placer [video](https://www.youtube.com/watch?v=UNqc_zMehAI) and [documentation](https://cookiebadger.github.io/assetplacer-docs/index.html)

## todo
* should we force items on a grid or not
* te
* duplicate selected item
* delete selected item
* box selection
* complete lister implementation
    * https://www.youtube.com/watch?v=XEzCy0c3gEU
* continous collision when placing items
* functionality so roate multiple can consider global/local pivot
* arrays
* snap to 3d splines:
    > This right here is the future. Bendable, rotatable, morphable 'grids'. Snap anything to anywhere. Precise measurements. Edit quickly, even after content is complete.
    * https://twitter.com/RYStorm/status/1012431708245905413
    * https://www.youtube.com/watch?v=hzhHJSb8OUI&feature=youtu.be
* townscaper grid: https://www.boristhebrave.com/docs/sylves/1/articles/tutorials/townscaper.html https://john-wigg.dev/SphereScaper/nodemo
* array generator: https://www.youtube.com/watch?v=WaocrCHqIlc&feature=youtu.be
* physics based placement: https://twitter.com/wilnyl/status/1067841938093735936
* prop libraries/kits
    * https://www.twitter.com/delaneykingrox/status/1504613050049232896
    * visual browser:
        * fuzzy search space=and
        * collection/tags editor
    * zoo/sprawl mode
        * default pack assets from library
        * simple toggle back and forth with space
* random placement on quad
* rotator tool with protractor thing from sketchup
* replace with matching mesh
* quickly switch between old snaps (switching between 1 and 3 for example)
* iterate selection when raycast return many
* update grid
        * progrids official: https://www.youtube.com/watch?v=1G-0f5m1_lw
        * progrids brackeys: https://www.youtube.com/watch?v=UtNvtIrJcNc
* specify custom grid
* change colors/themes between dark and light
* search ui commands (ctrl-p)
* see measurement directly for selected item, sketchup like
* multiple 3d windows
* cordon view/occulsion box - boxes to hide everything outside boxes (also in game) - (hammer)
* keys to rotate object around xyz with fixed increments
* hotkeys to toggle snapp
* alt key to temporarily toggle snap
* prefabs/pack ins (meshes, entities + lights)
    * https://www.gamasutra.com/blogs/NathanCheever/20180618/320112/A_Prefab_Primer.php
    * dynamic prefabs? handle scaling with winforms like anchor points
    * create prefab, sketchup style(component), share prefab between levels
    * https://www.youtube.com/watch?v=-AeUyNaWb58
* grid paint
* smart paint/wave function collapse to a grid
* plug/socket system, 2 mesh types that go together, can be from different kits as long as they match, square plugs, circle plugs etc...
  create for transition blocks or wall/destruction
* add decals for rust, mold and cracks
* mouse wheel swap (ctrl+scroll?)
* photoshop like layers
* prop kit => asset forge?

## todo (require better renderer)
* proper rendering of current selection
* render name and area for prop libraries
* hint color when hovering over multi selection
