<?php
/**
 * The template for displaying the header
 *
 * Displays all of the head element and everything up until the "site-content" div.
 *
 * @package KniffTech
 * @subpackage Bidnis
 * @since Bidnis 1.0
 */
?><!DOCTYPE html>
<html <?php language_attributes(); ?> class="no-js">
<head>
	<meta charset="<?php bloginfo( 'charset' ); ?>">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="profile" href="http://gmpg.org/xfn/11">
	<link rel="pingback" href="<?php bloginfo( 'pingback_url' ); ?>">
	<?php wp_head(); ?>
</head>
<body <?php body_class(); ?>>
	<header class="site-header">
		<div class="wrapper site-header-items-wrapper">
			<div class="site-identity">
				<?php if( function_exists( 'the_custom_logo' ) ) the_custom_logo(); ?>
				
				<?php if( get_theme_mod('display_site_title', true) ): ?>
					<h1 class="site-title">
						<a href="<?php echo esc_url( home_url( '/' ) ); ?>"><?php bloginfo( 'name' ); ?></a>
					</h1>
				<?php endif; ?>
			
				<?php if( get_theme_mod('display_tagline', true) ):?>
					<p class="site-tagline"><?php bloginfo( 'description' ) ?></p>	
				<?php endif; ?>
			</div>

			<?php bidnis_phone_email(); ?>

			<div class="social-bar">
				<div class="menu-toggle">
					<i class="fa fa-bars"></i>
				</div>
				<?php bidnis_social_media(); ?>
			</div>

		</div>

		<?php if ( has_nav_menu( 'header' )):
			wp_nav_menu( array(
				'theme_location'	 => 'header',
				'menu_class'     	 => 'header-menu wrapper',
				'container_class'	 => 'header-menu-container'
			) );
		endif; ?>

		<?php get_template_part('template-parts/header_image'); ?>

	</header>