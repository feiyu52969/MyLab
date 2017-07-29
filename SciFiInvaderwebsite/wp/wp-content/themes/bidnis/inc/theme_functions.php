<?php
/**
 * Custom theme functions
 *
 * @package KniffTech
 * @subpackage Bidnis
 * @since Bidnis 1.0
 */


/**
	* Social media icons
	*
	* @since Bidnis 1.0
	*/
$bidnis_social_media_icons = array(
	'Twitter'			=>	'fa-twitter',
	'Facebook'		=>	'fa-facebook',
	'Instagram'		=>	'fa-instagram',
	'Vine'				=>	'fa-vine',
	'SnapChat'		=>	'fa-snapchat-ghost',
	'LinkedIn'		=>	'fa-linkedin',
	'Google+'			=>	'fa-google-plus',
	'YouTube'			=>	'fa-youtube',
	'Twitch'			=>	'fa-twitch',
	'Vimeo'				=>	'fa-vimeo',
	'Pinterest'		=>	'fa-pinterest',
	'Reddit'			=>	'fa-reddit-alien',
	'Steam'				=>	'fa-steam',
	'Flickr'			=>	'fa-flickr',
	'Tumblr'			=>	'fa-tumblr',
	'Spotify'			=>	'fa-spotify',
	'Soundcloud'	=>	'fa-soundcloud',
	'MixCloud'		=>	'fa-mixcloud',
	'GitHub'			=>	'fa-github',
	'BitBucket'		=>	'fa-bitbucket',
	'Behance'			=>	'fa-behance',
	'LastFM'			=>	'fa-lastfm',
	'DeviantArt'	=>	'fa-deviantart',
	'BitCoin'			=>	'fa-btc',
);

function bidnis_any_social_icons(){
	global $bidnis_social_media_icons;

	if(get_theme_mod( 'social_media_rss' )) return true;

	foreach( $bidnis_social_media_icons as $service => $icon ){
		if( get_theme_mod( 'social_media_'.strtolower($service) ) ) return true;
	}

	return false;
}

function bidnis_social_media(){
	if( !bidnis_any_social_icons() ) return;

	global $bidnis_social_media_icons; ?>
	
	<div class="bidnis-social-media-links">
		<?php if(get_theme_mod( 'social_media_rss' )): ?>
			<a title="<?php esc_attr( bloginfo('rss2_url') ); ?>" href="<?php esc_url( bloginfo('rss2_url') ); ?>" target="_blank">
				<i class="fa fa-rss"></i>
			</a>
		<?php endif;

		foreach( $bidnis_social_media_icons as $service => $icon ):
			if( get_theme_mod( 'social_media_'.strtolower($service) ) ): ?>
				<a href="<?php echo esc_url( get_theme_mod( 'social_media_'.strtolower($service) ) ); ?>" target="_blank">
					<i class="fa <?php echo esc_attr( $icon ); ?>"></i>
				</a>
			<?php endif;
		endforeach; ?>
	</div>
<?php }

/**
	* Phone and E-mail
	*
	* @since Bidnis 1.0
	*/
function bidnis_phone_email(){ ?>
	<div class="bidnis-phone-email">
		<?php if( get_theme_mod('phone') ): ?>
			<a class="bidnis-phone" href="tel:<?php echo esc_url( get_theme_mod('phone') ); ?>"><?php echo esc_html( get_theme_mod('phone') ); ?></a>
		<?php endif; ?>

		<?php if( get_theme_mod('email') ): ?>
			<a class="bidnis-email" href="mailto:<?php echo esc_attr( get_theme_mod('email') ); ?>" target="_top"><?php echo esc_html( get_theme_mod('email') ); ?></a>
		<?php endif; ?>
	</div>

<?php }

/**
	* Add classes to body depending on custom changes and content
	*
	* @since Bidnis 1.0
	*/
function bidnis_body_classes( $classes ) {
	// Adds a class of custom-background-image to sites with a custom background image.
	if ( get_background_image() ) {
		$classes[] = 'custom-background-image';
	}

	// Adds a class of custom-header-image if a header image or content is set.
	if( get_theme_mod('header_image_text') || ( get_theme_mod('header_image_cta_text') && get_theme_mod('header_image_cta_url') ) ){
		$classes[] = 'custom-header-image-content';
	}

	// Adds a class of has-sidebar or no-sidebar depending on active sidebars.
	if ( is_active_sidebar( 'sidebar-left-widget-area' ) || is_active_sidebar( 'sidebar-right-widget-area' ) ) {
		$classes[] = 'has-sidebar';
	}else{
		$classes[] = 'no-sidebar';
	}

	return $classes;
}
add_filter( 'body_class', 'bidnis_body_classes' );

/**
	* Add classes to posts depending on custom changes and content
	*
	* @since Bidnis 1.0
	*/
function bidnis_post_classes( $classes ) {
	// adds a class of archive-entry to looped layout posts with excerpts
	if( !get_theme_mod( 'display_content', false ) && !is_singular() ){
		$classes[] =  'archive-entry';
	}

	return $classes;
}
add_filter( 'post_class', 'bidnis_post_classes' );

/**
	* Custom "Read more" on excerpts
	*
	* @since Bidnis 1.0
	*/
function bidnis_excerpt_read_more($more) {
	global $post;
	
	if( !get_theme_mod('read_more', true) ){
		return '... <a href="'. esc_url( get_permalink($post->ID) ) . '">Read more</a>';
	}else{
		return '...';
	}
}
add_filter('excerpt_more', 'bidnis_excerpt_read_more');

/**
	* Breadcrumbs
	*
	* @since Bidnis 1.0
	*/

function bidnis_breadcrumbs(){ 
	if( get_theme_mod('breadcrumbs', false) ): ?>
		<div class="bidnis-breadcrumbs">
			<?php // Home
				printf( '<span><a href="%1$s">%2$s</a></span>',
					esc_url( home_url() ),
					get_bloginfo('name')
				);
			?>

			<?php // Category
				if( is_singular() && has_category() ){ ?>
					<span><?php the_category(', '); ?></span> 
				<?php }elseif( is_archive() ){
					the_archive_title('<span>', '</span>');
				}
			?>

			<?php // Page hirearchy
				if( is_page() ){
					$ancestors = get_post_ancestors($post);

					if($ancestors){
						$ancestors = array_reverse($ancestors);

						foreach ($ancestors as $crumb) {
							printf( '<span><a href="%1$s">%2$s</a></span>',
								esc_url( get_permalink($crumb) ),
								get_the_title($crumb)
							);
						}
					}
				}
			?>

			<?php // Singular 
				if( is_singular() ){
					the_title('<span>', '</span>');
				}
			?>
		</div>
	<?php endif;
}