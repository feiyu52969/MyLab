jQuery(document).ready(function ($) {
	
    $('#site-navigation').meanmenu({
	    meanScreenWidth: "991",
	    meanRevealPosition: "center"
	});
    
    /** Variables from Customizer for Slider settings */
    if( elegant_pink_data.auto == '1' ){
        var slider_auto = true;
    }else{
        slider_auto = false;
    }
    
    if( elegant_pink_data.loop == '1' ){
        var slider_loop = true;
    }else{
        var slider_loop = false;
    }
    
    if( elegant_pink_data.option == '1' ){
        var slider_option = true;
    }else{
        slider_option = false;
    }
    /** Home Page Slider */
    $('#imageGallery').lightSlider({
        item : 1,
        slideMargin: 0,
        mode: elegant_pink_data.mode,
        speed: elegant_pink_data.speed, //ms'
        auto: slider_auto,
        loop: slider_loop,
        pause: elegant_pink_data.pause,
        controls: slider_option,
        pager: false,
        enableDrag:false,
        
    });
    
    /** Masonry */
    $('.ep-masonry').imagesLoaded(function(){ 
        $('.ep-masonry').masonry({
            itemSelector: '.post'
        }); 
    });
});