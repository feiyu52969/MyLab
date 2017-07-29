<?php
/**
 * Template for displaying posts
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
		<?php get_template_part('template-parts/content'); ?>

		<?php ( comments_open() || get_comments_number() ) ? comments_template() : false; ?>
	</main>

	<?php get_sidebar(); ?>
</div>

<?php get_footer(); ?>