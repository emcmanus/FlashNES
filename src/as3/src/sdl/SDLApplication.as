package sdl {
	
	import cmodule.libNES.CLibInit;
	
	import flash.display.DisplayObject;
	import flash.display.StageDisplayState;
	import flash.events.Event;
	import flash.events.SampleDataEvent;
	import flash.media.Sound;
	import flash.media.SoundChannel;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	import sdl.events.ListenerManager;
	import sdl.video.VideoSurface;
	
	/**
	 * This class contans the public interface for an SDL application.
	 */
	public class SDLApplication
	{
		/** @private */
		protected var videoSurface:VideoSurface;
		
		/** @private */
		protected var eventManager:ListenerManager;
		
		/** @default 320 */
		protected var SDLWidth:int = 320;
		
		/** @default 240 */
		protected var SDLHeight:int = 240;
		
		
		// Sound object we'll stream audio to
		protected var soundOut:Sound;
		protected var soundBuffer:ByteArray;	// AS3's sound buffer, not C's
		protected var soundChannel:SoundChannel;	// We use this to restart the sound object
		protected var soundBufferAddress:Number;
		
		// C Ram
		protected var domainMemory:ByteArray;
		
		/**
		 * @private
		 * 
		 * Loads and initializes the C library.
		 */
		internal var cLoader:CLibInit;
		
		/**
		 * Reference to the initialized C application. Dictionary of C callbacks.
		 */
		public var cLib:Object;
		
		
		/**
		 * Constructor. Following construction, use getSurface() to build an SDL video surface,
		 * and setEventTarget() to receive user input.
		 * 
		 * <p>If your application requires some special initialization process, add it here.</p>
		 */
		public function SDLApplication( romData:ByteArray, parent:DisplayObject )
		{
			this.cLoader = new CLibInit();
			
			// Select SDL drivers
			this.cLoader.putEnv( "SDL_VIDEODRIVER", "flash" );
//			this.cLoader.putEnv( "SDL_AUDIODRIVER", "flash" );
			
			
			// NOTES
			
			// We're no longer using SDL for audio.
			// May skip video, too.
			
			// Interestingly, if we leave in the audiodriver definition and *un*comment all SDL audio init code, the port runs significantly faster (30 instead of 20)
			// I guess if we try to init SDL audio *then* it actually checks if the audiodriver is set to flash. If it's not, then SDL will disable audio internally.
			
			// Shit! Now sound doesn't work.
			
			// Clearly sound wasn't being emulated. 30FPS is apparently the framerate without audio.
			
			this.cLoader.supplyFile( "balloonboy.rom", romData );
			
			this.cLib = cLoader.init();
			this.cLib.setup();
			
			
			// Init sound
			this.soundOut = new Sound;
			this.soundOut.addEventListener( SampleDataEvent.SAMPLE_DATA, onSoundSampleData );
			
			this.soundBuffer = new ByteArray;
			
			parent.stage.addEventListener(Event.ENTER_FRAME, onEnterFrame);
			
			
			// Ram
			var ram_NS:Namespace = new Namespace("cmodule.libNES");
			this.domainMemory = (ram_NS::gstate).ds;
		}
		
		// Tick
		protected function onEnterFrame(e:Event):void
		{
			if (!this.soundChannel)
			{
//				trace("\nrestart sound channel");
				
				this.soundChannel = soundOut.play();
				this.soundChannel.addEventListener(Event.SOUND_COMPLETE, onSoundComplete);
			}
		}
		
		
		protected function onSoundComplete(e:Event):void
		{
//			trace("sound complete");
			this.soundChannel.removeEventListener(Event.SOUND_COMPLETE, onSoundComplete);
			this.soundChannel = null;
		}
		
		
		protected function onSoundSampleData( e:SampleDataEvent ):void
		{
			// When the sound object wants more data...
			// Tell flash to empty its buffer into our sound object
			e.data.endian = Endian.LITTLE_ENDIAN;
			cLib.paintSound( e.data );
		}
		
		
		
		
		/**
		 * Initializes an SDL video surface and attaches required event listeners. Returned
		 * bitmap must be added to the display hierarchy.
		 * 
		 * <p>Application-specific code required to initialize the display should be added here.</p>
		 * 
		 * @param	displayWidth	The SDL video surface desired width. [Optional]
		 * @param	displayHeight	The SDL video surface desired height. [Optional]
		 * 
		 * @return	A bitmap mapped to the SDL Video Surface.
		 */
		public function getSurface( width:int=0, height:int=0 ):VideoSurface
		{
			if (!videoSurface)
			{
				this.SDLWidth = width;
				this.SDLHeight = height;
				
				cLib.setup( width, height );
				videoSurface = new VideoSurface( this, width, height );
			}
			
			return videoSurface;
		}
		
		/**
		 * Registers the given display object to receive keypress and mouse events. MouseMove events are
		 * recorded relative to this display object.
		 * 
		 * @param	eventTarget	The display object to register for keypress and mouse events.
		 */
		public function setEventTarget( eventTarget:DisplayObject ):void
		{
			if (!eventManager)
			{
				eventManager = new ListenerManager( eventTarget );
				cLib.setEventManager( eventManager );	// pass manager reference to c lib for event retrieval
			}
		}
		
		/**
		 * Toggle view states (fs/normal)
		 */
		public function toggleFullscreen():void {
			if ( videoSurface.stage )
			{
				if ( videoSurface.stage.displayState == StageDisplayState.FULL_SCREEN )
				{
					videoSurface.stage.displayState = StageDisplayState.NORMAL;
				}
				else
				{
					videoSurface.stage.displayState = StageDisplayState.FULL_SCREEN;
				}
			}
		}
	}
}