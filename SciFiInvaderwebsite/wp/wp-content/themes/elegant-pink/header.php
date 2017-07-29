<?php
/**
 * The header for our theme.
 *
 * This is the template that displays all of the <head> section and everything up until <div id="content">
 *
 * @link https://developer.wordpress.org/themes/basics/template-files/#template-partials
 *
 * @package Elegant_Pink
 */

?><!DOCTYPE html>
<html <?php language_attributes(); ?>>
<head>
<meta charset="<?php bloginfo( 'charset' ); ?>">
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="profile" href="http://gmpg.org/xfn/11">
<link rel="pingback" href="<?php bloginfo( 'pingback_url' ); ?>">

<?php wp_head(); ?>
</head>

<body <?php body_class(); ?>>
<div id="page" class="site">	

	<header id="masthead" class="site-header" role="banner">
		<div class="container">
			
            <div class="header-t">
    			<?php if( get_theme_mod( 'elegant_pink_ed_social', '1' ) ) do_action( 'elegant_pink_social' ); ?>
    			<?php get_search_form(); ?>
			</div>
            
            <div class="site-branding">
                <?php 
                if( function_exists( 'has_custom_logo' ) && has_custom_logo() ){
                    the_custom_logo();
                }
                ?>
                
                <h1 class="site-title"><a href="<?php echo esc_url( home_url( '/' ) ); ?>" rel="home"><?php bloginfo( 'name' ); ?></a></h1>
                <?php            
                $description = get_bloginfo( 'description', 'display' );
                if ( $description || is_customize_preview() ) { ?>
                <p class="site-description"><?php echo $description; /* WPCS: xss ok. */ ?></p>
                <?php } ?>            
            </div><!-- .site-branding -->
            
        </div><!-- .container -->
   </header><!-- #masthead -->
   
    <div class="nav">
        <div class="container">         
            <nav id="site-navigation" class="main-navigation" role="navigation">
		  	   <button class="menu-toggle" aria-controls="primary-menu" aria-expanded="false"><?php esc_html_e( 'Primary Menu', 'elegant-pink' ); ?></button>
                <?php wp_nav_menu( array( 'theme_location' => 'primary', 'menu_id' => 'primary-menu' ) ); ?>
            </nav><!-- #site-navigation -->
        </div>
    </div>
    
    <?php 
        if ( is_front_page() && ! is_home() ) {
            if( get_theme_mod( 'elegant_pink_ed_slider' ) ) do_action( 'elegant_pink_slider' );
        }    
    ?>
    
    <div class="container">
        <?php if( ! is_404() ) { ?>
            <div id="content" class="site-content">
        <?php } ?>
