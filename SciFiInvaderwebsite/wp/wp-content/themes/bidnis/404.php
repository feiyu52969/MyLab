<?php
/**
 * Template for displaying 404 pages (not found)
 *
 * @package KniffTech
 * @subpackage Bidnis
 * @since Bidnis 1.0
 */
get_header(); ?>

<?php get_sidebar('above'); ?>

<div class="site-content wrapper">
	<?php get_sidebar('left'); ?>

	<main>
		<?php get_template_part('template-parts/content_header'); ?>
		
		<p><?php _e( 'This is somewhat embarrassing, isn&rsquo;t it?', 'bidnis' ); ?></p>
		<p><?php _e( 'It looks like nothing was found at this location.', 'bidnis' ); ?></p>
	</main>

	<?php get_sidebar(); ?>
</div>

<?php get_footer(); ?>