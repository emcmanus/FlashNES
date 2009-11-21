package sdl.video {
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.events.Event;
	import flash.geom.Rectangle;
	import flash.utils.ByteArray;
	
	import sdl.SDLApplication;
	
	/**
	 * @private
	 * 
	 * A flash representation of SDL_VideoSurface. This class initializes itself around videoData.
	 */
	public class VideoSurface extends Bitmap {
		
		private var screenRectangle:Rectangle;
		
		private var videoSurfaceData:BitmapData;
		private var domainMemory:ByteArray;	// The C virtual machine's RAM
		
		internal var sdlApplication:SDLApplication;
		
		private var displayBufferAddress:Number;
		
		
		/**
		 * Initialize local variables and construct the Bitmap using videoSurfaceData bitmapData
		 */
		
		public function VideoSurface( sdlApplication:SDLApplication, width:int, height:int )
		{
			// Init
			var ram_NS:Namespace = new Namespace("cmodule.libNES");
			this.domainMemory = (ram_NS::gstate).ds;
			this.videoSurfaceData = new BitmapData( width, height, false, 0x0 );
			this.screenRectangle = new Rectangle( 0, 0, width, height );
			
			this.sdlApplication = sdlApplication;
			
			// Pass bitmapData to the super constructor
			super( videoSurfaceData );
			
			this.addEventListener( Event.ADDED_TO_STAGE, onAddedToStage );
		}
		
		
		/**
		 * Added to stage
		 */
		internal function onAddedToStage( e:Event ):void
		{
			// Attach update timer
			this.stage.addEventListener( Event.ENTER_FRAME, onEnterFrame );
		}
		
		
		/**
		 * Frame-based operations
		 */
		
		internal function onEnterFrame( e:Event ):void
		{
			// Render next frame
			sdlApplication.cLib.tick();
			
			// Display invalidated
			updateDisplay();
		}
		
		
		/**
		 * Update ram pointer and copy pixels into videoSurfaceData.
		 */
		
		internal function updateDisplay():void
		{
			// Get buffer address once
			if ( !this.displayBufferAddress )
			{
				this.displayBufferAddress = this.sdlApplication.cLib.getDisplayPointer();
			}
			domainMemory.position = this.displayBufferAddress;
			videoSurfaceData.setPixels( screenRectangle, domainMemory );
		}
		
	}
}