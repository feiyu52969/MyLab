(function($){

	var bidnis = {
		sizes: {
			'desktop': 1280,
			'tablet': 640,
			'phone': 320
		},

		init: function(){
			this.cacheDom();
			this.bindEvents();

			this.$html.removeClass('no-js').addClass('js');

			this.resizeHandler();
			this.archiveTitlesFix();
			this.fullWidthVideoEmbeds();
		},

		cacheDom: function(){
			this.$window = $(window);
			this.$document = $(document);
			this.$htmlAndBody = this.$document.find('html, body');
			this.$html = this.$document.find('html');
			this.$scrollToTop = this.$document.find('.scroll-to-top');
			this.$menuHeaderToggle = this.$document.find('.menu-toggle');
			this.$menuHeaderContainer = this.$document.find('.header-menu-container');
			this.$videoPosts = this.$document.find('.single-format-video .post');
			this.$videoEmbeds = this.$videoPosts.find('iframe, video, embed, object, .video-player, .videopress-placeholder');
			this.$archiveTitle = this.$document.find('.archive .content-title');
		},

		bindEvents: function(){
			this.$window.resize( this.resizeHandler.bind(this) );
			this.$scrollToTop.on( 'click', this.scrollToTop.bind(this) );
			this.$menuHeaderToggle.on( 'click', this.toggleMenu.bind(this) );
			this.$document.on( 'scroll', this.scrollHandler.bind(this) );
		},

		toggleMenu: function(){
			this.$menuHeaderContainer.toggle()
		},

		scrollToTop: function(){
			this.$htmlAndBody.animate({scrollTop:0}, 'slow');
			return false;
		},

		resizeHandler: function(){
			var mm = window.matchMedia( '(max-width: '+this.sizes.tablet+'px)' );

			if( mm.matches ){
				this.$menuHeaderContainer.hide();
			}else{
				this.$menuHeaderContainer.show();
			}

			this.fullWidthVideoEmbeds();
		},

		fullWidthVideoEmbeds: function(){
			if( this.$videoEmbeds.length ){
				for( var x=0; x<this.$videoEmbeds.length; x++){
					var $video = $( this.$videoEmbeds[x] );

					$video.width('100%');
					$video.height( ($video.width()/16) * 9 );
				}
			}
		},

		scrollHandler: function(){
			if( this.$document.scrollTop() >= 500 ){
				this.$scrollToTop.show(500)
			}else{
				this.$scrollToTop.hide(500);
			}
		},

		archiveTitlesFix: function(){
			if( this.$archiveTitle.length ){
				var archiveTitle = this.$archiveTitle.html().split(': ');

				if( archiveTitle.length > 1 ){
					archiveTitle.shift();
					this.$archiveTitle.html( archiveTitle.join(': ') );
				}
			}
		}
	}

	$(document).ready( bidnis.init.bind(bidnis) );

})(jQuery);