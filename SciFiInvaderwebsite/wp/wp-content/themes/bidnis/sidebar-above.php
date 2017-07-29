<?php
/**
 * Template for displaying a widget area and additional content
 *
 * @package KniffTech
 * @subpackage Bidnis
 * @since Bidnis 1.0
 */

if ( is_active_sidebar( 'above-content-widget-area' )  ) : ?>
	<div class="above-content-widget-area sidebar widget-area wrapper" role="complementary">
		<?php dynamic_sidebar( 'above-content-widget-area' ); ?>
	</div><!-- .above-content-widget-area .sidebar .widget-area -->
<?php endif; ?>