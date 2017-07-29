<?php
/**
 * Template for displaying a widget area and additional content
 *
 * @package KniffTech
 * @subpackage Bidnis
 * @since Bidnis 1.0
 */

if ( is_active_sidebar( 'sidebar-left-widget-area' )  ) : ?>
	<aside class="sidebar-left-widget-area sidebar widget-area" role="complementary">
		<?php dynamic_sidebar( 'sidebar-left-widget-area' ); ?>
	</aside><!-- .sidebar-left-widget-area .sidebar .widget-area -->
<?php endif; ?>