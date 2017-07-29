<?php
/**
	* Bidnis Functions
	*
	* This functions file contains the the mainly used features of a WordPress theme
	* Uncomment or delete lines as needed
	*
	* @package KniffTech
	* @subpackage Bidnis
	* @since Bidnis 1.0
	*/

if ( ! function_exists( 'bidnis_setup' ) ) :
	/**
	* Initial theme setup
	*
	* @since Bidnis 1.0
	*/
	function bidnis_setup() {
		
		// Support for translation files
		load_theme_textdomain( 'bidnis', get_template_directory() . '/languages' );

		// Content width
		if ( ! isset( $content_width ) ) $content_width = 1280;

		// Adds editor styles
		add_editor_style( str_replace( ',', '%2C', 'https://fonts.googleapis.com/css?family=Open+Sans:400,600,800,800italic,700italic,700,600italic,400italic,300italic,300' ) );
		add_editor_style();

		// Enables support for Post Formats
		add_theme_support( 'post-formats', array(
			'image',
			'video',
			'quote',
			'link',
			'gallery',
			'status',
			'audio',
			'chat',
		) );

		// Enables support for custom logo
		add_theme_support( 'custom-logo' );

		//Enables support for Post Thumbnails on posts, pages and more
		add_theme_support( 'post-thumbnails' );

		//Support for custom background settings
		add_theme_support( 'custom-background', array(
			'default-color'			 => 'ffffff',
		) );

		//Support for custom header settings
		add_theme_support( 'custom-header', array(
			'default-image'						=> '%s/assets/images/header.jpg',
			'random-default'					=> false,
			'width'										=> 1920,
			'height'									=> 960,
			'flex-height'							=> false,
			'flex-width'							=> false,
			'header-text'							=> false,
			'uploads'					 				=> true,
		) );

		// Default custom headers packaged with the theme.
		register_default_headers( array(
			'vaping360' => array(
				'url'           => '%s/assets/images/header.jpg',
				'thumbnail_url' => '%s/assets/images/header.jpg',
				'description'   => _x( 'laptop', 'Laptop', 'bidnis' )
			)
		) );

		// Adds RSS feed in head
		add_theme_support( 'automatic-feed-links' );
		
		// Make core WordPress makup output valid HTML5
		add_theme_support( 'html5', array(
			'search-form',
			'comment-form',
			'comment-list',
			'gallery',
			'caption',
		) );
		
		// <title> tag provided by WordPress
		add_theme_support( 'title-tag' );

		// wp_nav_menu() setup
		register_nav_menus( array(
			'header' => __('Header Menu', 'bidnis'),
			'footer' => __('Footer Menu', 'bidnis')
		));
	}
endif;
add_action( 'after_setup_theme', 'bidnis_setup' );

/**
	* Basic widget area structure
	*
	* @since Bidnis 1.0
	*/
function bidnis_widget_init(){
	register_sidebar( array(
		'name' 					=> __('Above content', 'bidnis'),
		'id' 						=> 'above-content-widget-area',
		'description'		=> __('Widget area above the content', 'bidnis'),
		'before_widget' => '<div id="%1$s" class="widget-container %2$s">',
		'after_widget' 	=> '</div>',
		'before_title' 	=> '<h3 class="widget-title">',
		'after_title' 	=> '</h3>',
	) );

	register_sidebar( array(
		'name' 					=> __('Left Sidebar', 'bidnis'),
		'id' 						=> 'sidebar-left-widget-area',
		'description'		=> __('Widget area at the left side of the content', 'bidnis'),
		'before_widget' => '<div id="%1$s" class="widget-container %2$s">',
		'after_widget' 	=> '</div>',
		'before_title' 	=> '<h3 class="widget-title">',
		'after_title' 	=> '</h3>',
	) );

	register_sidebar( array(
		'name' 					=> __('Right Sidebar', 'bidnis'),
		'id' 						=> 'sidebar-right-widget-area',
		'description'		=> __('Widget area at the right side of the content', 'bidnis'),
		'before_widget' => '<div id="%1$s" class="widget-container %2$s">',
		'after_widget' 	=> '</div>',
		'before_title' 	=> '<h3 class="widget-title">',
		'after_title' 	=> '</h3>',
	) );

	register_sidebar( array(
		'name' 					=> __('Footer Top', 'bidnis'),
		'id' 						=> 'footer-top-widget-area',
		'description'		=> __('Widget area in the footer', 'bidnis'),
		'before_widget' => '<div id="%1$s" class="widget-container %2$s">',
		'after_widget' 	=> '</div>',
		'before_title' 	=> '<h3 class="widget-title">',
		'after_title' 	=> '</h3>',
	) );

	register_sidebar( array(
		'name' 					=> __('Footer One', 'bidnis'),
		'id' 						=> 'footer-one-widget-area',
		'description'		=> __('Widget area in the footer', 'bidnis'),
		'before_widget' => '<div id="%1$s" class="widget-container %2$s">',
		'after_widget' 	=> '</div>',
		'before_title' 	=> '<h3 class="widget-title">',
		'after_title' 	=> '</h3>',
	) );

	register_sidebar( array(
		'name' 					=> __('Footer Two', 'bidnis'),
		'id' 						=> 'footer-two-widget-area',
		'description'		=> __('Widget area in the footer', 'bidnis'),
		'before_widget' => '<div id="%1$s" class="widget-container %2$s">',
		'after_widget' 	=> '</div>',
		'before_title' 	=> '<h3 class="widget-title">',
		'after_title' 	=> '</h3>',
	) );

	register_sidebar( array(
		'name' 					=> __('Footer Three', 'bidnis'),
		'id' 						=> 'footer-three-widget-area',
		'description'		=> __('Widget area in the footer', 'bidnis'),
		'before_widget' => '<div id="%1$s" class="widget-container %2$s">',
		'after_widget' 	=> '</div>',
		'before_title' 	=> '<h3 class="widget-title">',
		'after_title' 	=> '</h3>',
	) );

	register_sidebar( array(
		'name' 					=> __('Footer Four', 'bidnis'),
		'id' 						=> 'footer-four-widget-area',
		'description'		=> __('Widget area in the footer', 'bidnis'),
		'before_widget' => '<div id="%1$s" class="widget-container %2$s">',
		'after_widget' 	=> '</div>',
		'before_title' 	=> '<h3 class="widget-title">',
		'after_title' 	=> '</h3>',
	) );
}
add_action( 'widgets_init', 'bidnis_widget_init' );


/**
	* Scripts, Styles and fonts
	*
	* @since Bidnis 1.0
	*/
function bidnis_scripts(){
	// Enqueue the font Open Sans from google
	wp_enqueue_style( 'bidnis-fonts', 'https://fonts.googleapis.com/css?family=Open+Sans:400,600,800,800italic,700italic,700,600italic,400italic,300italic,300', array(), null);

	// Enqueue Font Awesome (example icon set)
	wp_enqueue_style( 'font-awesome', get_template_directory_uri() . '/lib/font-awesome/css/font-awesome.min.css', array(), '4.6.3');

	// Enqueue style.css from root theme folder
	wp_enqueue_style( 'bidnis-style', get_stylesheet_uri() );

	// Enqueue theme JS and include jQuery
	wp_enqueue_script( 'bidnis-script', get_template_directory_uri() . '/js/functions.js', array( 'jquery' ), wp_get_theme()->get('Version'), true );

	// Comment-reply script
	if ( (!is_admin()) && is_singular() && comments_open() && get_option('thread_comments') ){
		wp_enqueue_script( 'comment-reply' );
	}
}
add_action( 'wp_enqueue_scripts', 'bidnis_scripts' );


/**
	* Customized CSS options
	*
	* @since Bidnis 1.1.1
	*/
function bidnis_customize_styles(){
	$options = [
		get_theme_mod('color_scheme', '#ffd200'),
		'#'.get_background_color(),
		get_theme_mod('text_color', '#191919'),
		get_theme_mod('header-text-color', '#191919'),
		get_theme_mod('link_color', '#191919')
	];

	$css = '
		*{
			border-color: %1$s !important;
		}

		body{
			background-color: %2$s;
		}

		a{ color: %5$s; }

		body,
		.archive-entry-header h3 a,
		.read-more,
		.comment-reply-link,
		.nav-previous a,
		.nav-next a,
		.page-numbers:not(.dots),
		.pagination,
		.pagination a{
			color: %3$s;
		}

		.site-header,
		.header-menu .menu-item:hover,
		.header-menu .current-menu-item,
		.header-image-cta,
		input[type="submit"]:hover,
		input[type="reset"]:hover,
		button:hover,
		.comment-reply-link:hover,
		.read-more:hover,
		.nav-previous a:hover,
		.nav-next a:hover,
		.page-numbers:not(.dots):hover,
		.scroll-to-top:hover,
		.page-numbers.current,
		.pagination > span:not(.page-links-title),
		.pagination > a:hover{
			background-color: %1$s;
		}

		.site-header,
		.site-header a,
		.site-tagline,
		.header-menu .menu-item:hover > a,
		.header-menu .current-menu-item > a{
			color: %4$s; 
		}
	';

	wp_add_inline_style( 'bidnis-style', vsprintf($css, $options) );
}
add_action( 'wp_enqueue_scripts', 'bidnis_customize_styles' );


/**
	* Included files
	* - Theme functions
	* - WP-Customize
	*
	* @since Bidnis 1.0
	*/
require get_template_directory() . '/inc/theme_functions.php';
require get_template_directory() . '/inc/customize.php';

?>