package {
	import flash.display.LoaderInfo;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	
	import utils.FPSCounter;
	
	import sdl.SDLApplication;
	import sdl.video.VideoSurface;
	
	[SWF(width=800,height=600,frameRate=60)]
	public class flashnes extends Sprite {
		
		private var surface:VideoSurface;
		private var surfaceContainer:Sprite;
		
		private var sdlApplication:SDLApplication;
		
		private var romLoader:URLLoader;
		
		public function flashnes()
		{
			// Get ROM Bytes
			this.romLoader = new URLLoader();
			this.romLoader.dataFormat = URLLoaderDataFormat.BINARY;
			this.romLoader.addEventListener( Event.COMPLETE, onRomLoaded );
			
			var paramObj:Object = LoaderInfo(this.root.loaderInfo).parameters;
			var romString:String = parseRom( paramObj ); // Use later
			
			if ( !romString )
			{
				// Testing
				romString = "http://localhost/test_roms/nes/1.nes";
			}
			
			this.romLoader.load(new URLRequest( romString ));
		}
		
		
		private function onRomLoaded(e:Event):void
		{
			// C Library setup
			this.sdlApplication = new SDLApplication( romLoader.data as ByteArray );
			this.surface = sdlApplication.getSurface( 256, 224 );
			
			surface.scaleX = 2;
			surface.scaleY = 2;
			
			// Build container for input events
			surfaceContainer = new Sprite();
			surfaceContainer.addChild( surface );
			addChild(surfaceContainer);
			
			sdlApplication.setEventTarget( surfaceContainer );
			
			// Monitor FPS performance
			var fps:FPSCounter = new FPSCounter();
			addChild(fps);
		}
		
		
		public function parseRom(paramObj:Object):String
		{
			var keyStr:String = "";
			var valueStr:String = "";
			
			for (keyStr in paramObj) {
				valueStr = String(paramObj[keyStr]);
				
				switch(keyStr)
				{
					case "rom":
						return valueStr;
						break;
				}
			}
			return "";
		}
		
	}
}
