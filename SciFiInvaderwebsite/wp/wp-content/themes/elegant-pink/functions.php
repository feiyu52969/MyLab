<?php
/**
 * Elegant Pink functions and definitions.
 *
 * @link https://developer.wordpress.org/themes/basics/theme-functions/
 *
 * @package Elegant_Pink
 */

if ( ! function_exists( 'elegant_pink_setup' ) ) :
/**
 * Sets up theme defaults and registers support for various WordPress features.
 *
 * Note that this function is hooked into the after_setup_theme hook, which
 * runs before the init hook. The init hook is too late for some features, such
 * as indicating support for post thumbnails.
 */
function elegant_pink_setup() {
	/*
	 * Make theme available for translation.
	 * Translations can be filed in the /languages/ directory.
	 * If you're building a theme based on Elegant Pink, use a find and replace
	 * to change 'elegant-pink' to the name of your theme in all the template files.
	 */
	load_theme_textdomain( 'elegant-pink', get_template_directory() . '/languages' );
    
    // Add default posts and comments RSS feed links to head.
	add_theme_support( 'automatic-feed-links' );

	/*
	 * Let WordPress manage the document title.
	 * By adding theme support, we declare that this theme does not use a
	 * hard-coded <title> tag in the document head, and expect WordPress to
	 * provide it for us.
	 */
	add_theme_support( 'title-tag' );

	/*
	 * Enable support for Post Thumbnails on posts and pages.
	 *
	 * @link https://developer.wordpress.org/themes/functionality/featured-images-post-thumbnails/
	 */
	add_theme_support( 'post-thumbnails' );

	// This theme uses wp_nav_menu() in one location.
	register_nav_menus( array(
		'primary' => esc_html__( 'Primary', 'elegant-pink' ),
	) );

	/*
	 * Switch default core markup for search form, comment form, and comments
	 * to output valid HTML5.
	 */
	add_theme_support( 'html5', array(
		'search-form',
		'comment-form',
		'comment-list',
		'gallery',
		'caption',
	) );

	/*
	 * Enable support for Post Formats.
	 * See https://developer.wordpress.org/themes/functionality/post-formats/
	 */
	add_theme_support( 'post-formats', array(
		'aside',
		'image',
		'video',
		'quote',
		'link',
	) );

	/* Set up the WordPress core custom background feature. */
	add_theme_support( 'custom-background', apply_filters( 'elegant_pink_custom_background_args', array(
		'default-color' => 'ffffff',
		'default-image' => '',
	) ) );
    
    /* Custom Image Sizes */
    add_image_size( 'elegant-pink-post-thumb', 80, 66, true);
    add_image_size( 'elegant-pink-slider', 1800, 696, true);
    add_image_size( 'elegant-pink-image', 780, 500, true);
    add_image_size( 'elegant-pink-image-full', 1180, 500, true);
    add_image_size( 'elegant-pink-featured-image', 280, 250, true);
    
    /* Custom Logo */
    add_theme_support( 'custom-logo', array(
    	'header-text' => array( 'site-title', 'site-description' ),
    ) );
}
endif;
add_action( 'after_setup_theme', 'elegant_pink_setup' );

/**
 * Set the content width in pixels, based on the theme's design and stylesheet.
 *
 * Priority 0 to make it available to lower priority callbacks.
 *
 * @global int $content_width
 */
function elegant_pink_content_width() {
	$GLOBALS['content_width'] = apply_filters( 'elegant_pink_content_width', 780 );
}
add_action( 'after_setup_theme', 'elegant_pink_content_width', 0 );

/**
* Adjust content_width value according to template.
*
* @return void
*/
function elegant_pink_template_redirect_content_width() {

	// Full Width in the absence of sidebar.
	if( is_page() ){
	   $sidebar_layout = elegant_pink_sidebar_layout();
       if( ( $sidebar_layout == 'no-sidebar' ) || ! ( is_active_sidebar( 'right-sidebar' ) ) ) $GLOBALS['content_width'] = 1180;
        
	}elseif ( ! ( is_active_sidebar( 'right-sidebar' ) ) ) {
		$GLOBALS['content_width'] = 1180;
	}

}
add_action( 'template_redirect', 'elegant_pink_template_redirect_content_width' );

/**
 * Register widget area.
 *
 * @link https://developer.wordpress.org/themes/functionality/sidebars/#registering-a-sidebar
 */
function elegant_pink_widgets_init() {
	register_sidebar( array(
		'name'          => esc_html__( 'Right Sidebar', 'elegant-pink' ),
		'id'            => 'right-sidebar',
		'description'   => '',
		'before_widget' => '<section id="%1$s" class="widget %2$s">',
		'after_widget'  => '</section>',
		'before_title'  => '<h2 class="widget-title">',
		'after_title'   => '</h2>',
	) );
    
    register_sidebar( array(
		'name'          => esc_html__( 'Footer One', 'elegant-pink' ),
		'id'            => 'footer-one',
		'description'   => '',
		'before_widget' => '<section id="%1$s" class="widget %2$s">',
		'after_widget'  => '</section>',
		'before_title'  => '<h2 class="widget-title">',
		'after_title'   => '</h2>',
	) );
    
    register_sidebar( array(
		'name'          => esc_html__( 'Footer Two', 'elegant-pink' ),
		'id'            => 'footer-two',
		'description'   => '',
		'before_widget' => '<section id="%1$s" class="widget %2$s">',
		'after_widget'  => '</section>',
		'before_title'  => '<h2 class="widget-title">',
		'after_title'   => '</h2>',
	) );
    
    register_sidebar( array(
		'name'          => esc_html__( 'Footer Three', 'elegant-pink' ),
		'id'            => 'footer-three',
		'description'   => '',
		'before_widget' => '<section id="%1$s" class="widget %2$s">',
		'after_widget'  => '</section>',
		'before_title'  => '<h2 class="widget-title">',
		'after_title'   => '</h2>',
	) );
}
add_action( 'widgets_init', 'elegant_pink_widgets_init' );

/**
 * Enqueue scripts and styles.
 */
function elegant_pink_scripts() {
	$my_theme = wp_get_theme();
    $version = $my_theme['Version'];
    
    $elegant_pink_query_args = array(
		'family' => 'Merriweather:400,400italic,700,700italic|Roboto:400,700,900,500|Dancing Script:400,700',
		);
    
    wp_enqueue_style( 'elegant-pink-font-awesome', get_template_directory_uri(). '/css/font-awesome.css' );
    wp_enqueue_style( 'elegant-pink-lightslider-style', get_template_directory_uri(). '/css/lightslider.css' );
    wp_enqueue_style( 'elegant-pink-meanmenu-style', get_template_directory_uri(). '/css/meanmenu.css' );
    wp_enqueue_style( 'elegant-pink-google-fonts', add_query_arg( $elegant_pink_query_args, "//fonts.googleapis.com/css" ) );
    wp_enqueue_style( 'elegant-pink-style', get_stylesheet_uri(), '', $version );
    
    wp_enqueue_script( 'elegant-pink-meanmenu', get_template_directory_uri() . '/js/jquery.meanmenu.js', array('jquery'), '2.0.8', true );
    wp_enqueue_script( 'elegant-pink-lightslider', get_template_directory_uri() . '/js/lightslider.js', array('jquery'), '1.1.5', true );    
    wp_enqueue_script( 'masonry' );
    wp_register_script( 'elegant-pink-custom-js', get_template_directory_uri() . '/js/custom.js', array('jquery'), $version, true );
	wp_register_script( 'elegant-pink-ajax', get_template_directory_uri() . '/js/ajax.js', array('jquery'), $version, true );
    
    $elegant_pink_slider_auto = get_theme_mod( 'elegant_pink_slider_auto', '1' );
    $elegant_pink_slider_loop = get_theme_mod( 'elegant_pink_slider_loop', '1' );
    $elegant_pink_slider_control = get_theme_mod( 'elegant_pink_slider_control', '1' );
    $elegant_pink_slider_transition = get_theme_mod( 'elegant_pink_slider_transition', 'fade' );
    $elegant_pink_slider_speed = get_theme_mod( 'elegant_pink_slider_speed', '400' );
    $elegant_pink_slider_pause = get_theme_mod( 'elegant_pink_slider_pause', '6000' );
    
    $elegant_pink_translation_array = array(
		'auto'   => esc_attr($elegant_pink_slider_auto),
		'loop' => esc_attr($elegant_pink_slider_loop),
        'option' => esc_attr($elegant_pink_slider_control),
        'mode' => esc_attr($elegant_pink_slider_transition),
        'speed' => absint($elegant_pink_slider_speed),
		'pause'  => absint($elegant_pink_slider_pause),
        'ajax_url' => admin_url( 'admin-ajax.php' ),
        'elegant_pink_nonce' => wp_create_nonce( 'elegant_pink_nonce' )
		);
    wp_localize_script( 'elegant-pink-custom-js', 'elegant_pink_data', $elegant_pink_translation_array );
    wp_enqueue_script( 'elegant-pink-custom-js' );
    
    $pagination = get_theme_mod( 'elegant_pink_pagination_type', 'default' );
    
    if( $pagination == 'load_more' ){
        
        // Add parameters for the JS
        
        if( is_page_template( 'template-home.php' ) ){
            $paged = ( get_query_var( 'page' ) > 1 ) ? get_query_var( 'page' ) : 1;
            $blog_qry = new WP_Query( array( 'post_type'=>'post', 'paged'=>$paged ) );
            $max = $blog_qry->max_num_pages;
            wp_reset_postdata();
        }else{
            global $wp_query;
            $max = $wp_query->max_num_pages;
            $paged = ( get_query_var( 'paged' ) > 1 ) ? get_query_var( 'paged' ) : 1;
        }
        
        wp_enqueue_script( 'elegant-pink-ajax' );
        
        wp_localize_script( 
            'elegant-pink-ajax', 
            'elegant_pink_ajax',
            array(
                'startPage'     => $paged,
                'maxPages'      => $max,
                'nextLink'      => next_posts( $max, false ),
                'autoLoad'      => $pagination,
                'loadmore'      => __( 'Load More Posts', 'elegant-pink' ),
                'loading'       => __('Loading...', 'elegant-pink'),
                'nomore'        => __( 'No more posts.', 'elegant-pink' ),
                'plugin_url'    => plugins_url()
             )
        );
        
        if ( is_jetpack_activated( true ) ) {
            wp_enqueue_style( 'tiled-gallery-css', plugins_url() . '/jetpack/modules/tiled-gallery/tiled-gallery/tiled-gallery.css' );            
        }
    }
    
	if ( is_singular() && comments_open() && get_option( 'thread_comments' ) ) {
		wp_enqueue_script( 'comment-reply' );
	}
}
add_action( 'wp_enqueue_scripts', 'elegant_pink_scripts' );

if ( is_admin() ) : // Load only if we are viewing an admin page
function elegant_pink_admin_scripts() {
	wp_enqueue_style( 'elegant-pink-admin-style',get_template_directory_uri().'/inc/css/admin.css', '1.0', 'screen' );
    
    wp_enqueue_script( 'elegant-pink-admin-js', get_template_directory_uri().'/inc/js/admin.js', array( 'jquery' ), '', true );
    wp_localize_script( 'elegant-pink-admin-js', 'elegant_pink_data', array( 'promo_url' => get_template_directory_uri() . '/images/upgrade.png' ) );
	
}
add_action( 'admin_enqueue_scripts', 'elegant_pink_admin_scripts' );
endif;

/**
 * Custom template tags for this theme.
 */
require get_template_directory() . '/inc/template-tags.php';

/**
 * Custom functions that act independently of the theme templates.
 */
require get_template_directory() . '/inc/extras.php';

/**
 * Customizer additions.
 */
require get_template_directory() . '/inc/customizer.php';

/**
 * Load Jetpack compatibility file.
 */
require get_template_directory() . '/inc/jetpack.php';

/**
 * Featured Post Widget
 */
require get_template_directory() . '/inc/widget-featured-post.php';

/**
 * Recent Post Widget
 */
require get_template_directory() . '/inc/widget-recent-post.php';

/**
 * Popular Post Widget
 */
require get_template_directory() . '/inc/widget-popular-post.php';

/**
 * Social Link Widget
 */
require get_template_directory() . '/inc/widget-social-links.php';

/**
 * Add Custom Meta Box
 */
require get_template_directory() . '/inc/metabox.php';
