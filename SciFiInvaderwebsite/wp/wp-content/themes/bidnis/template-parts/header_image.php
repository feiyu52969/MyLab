<?php
/**
 * The template part for displaying the header image, text and call-to-action
 *
 * @package KniffTech
 * @subpackage Bidnis
 * @since Bidnis 1.0
 */
if( !is_home() && !is_front_page() ){
	return;
} ?>

<div class="header-image-container">
	<?php if( has_header_image() ): ?>
		<img  src="<?php header_image(); ?>" 
			width="<?php echo get_custom_header()->width; ?>"
			height="<?php echo get_custom_header()->height; ?>" 
			alt="<?php _e('Header Image', 'bidnis');?>" />
	<?php endif; ?>

	<?php if( get_theme_mod('header_image_text') ||
		( get_theme_mod('header_image_cta_text') && get_theme_mod('header_image_cta_url') ) ):?>
		<div class="header-image-inner wrapper">
			<div>
				<?php if( get_theme_mod('header_image_text') ){
					printf( '<h1 class="header-image-text">%s</h1>', esc_html( get_theme_mod('header_image_text') ) );
				} ?>

				<?php if( get_theme_mod('header_image_cta_text') && get_theme_mod('header_image_cta_url') ){
					printf( '<a class="header-image-cta" href="%1$s">%2$s</a>',
						esc_url( get_theme_mod('header_image_cta_url') ),
						esc_html( get_theme_mod('header_image_cta_text') )
					);
				} ?>
			</div>
		</div>
	<?php endif; ?>
</div>