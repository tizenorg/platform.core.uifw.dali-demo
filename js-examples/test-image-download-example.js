var window= {
        x:0,
         y:0,
       width:1920, height: 1080,	// my monitor
//       width:720, height: 1280,	// z3
       transparent: false,
       name:'hello-button'
 };

var viewMode={
	'stereoscopicMode':'mono', // stereo-horizontal, stereo-vertical, stereo-interlaced,
	'stereoBase': 65 // Distance in millimeters between left/right cameras typically between (50-70mm)
};
  
var options= {
	'window': window,
	'viewMode': viewMode,
};
  
var dali = require('dali')( options );


var daliApp = {};

daliApp.init = function() {
	dali.stage.setBackgroundColor(dali.COLOR_WHITE);

  imagePaths = ['http://www.w3schools.com/images/colorpicker.gif',
                'https://www.google.co.kr/logos/doodles/2015/holidays-2015-day-1-6575248619077632-res.png',
             ];

  for(i=0; i<imagePaths.length; ++i)
  {
    var imageView = new dali.Control('ImageView');
    imageView.parentOrigin = dali.TOP_LEFT;
    imageView.anchorPoint = dali.TOP_LEFT;
    imageView.image = imagePaths[i];
    imageView.size = [100,100,0];
    imageView.position = [100*i,0,0];
    dali.stage.add(imageView);
  }
}

daliApp.init();

