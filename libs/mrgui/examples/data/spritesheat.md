Kenney ui sprites

fed into this tool to generate the spritesheet
https://www.codeandweb.com/free-sprite-sheet-packer

and a json file that is imported and converted to cpp with the following code:
`console.log(Object.entries(data.frames).map(([k,v]) => { return `constexpr Rect ${k.substr(0, k.length-4)} = img(${v.frame.x}, ${v.frame.y}, ${v.frame.w}, ${v.frame.h});\n`}).join(''))`