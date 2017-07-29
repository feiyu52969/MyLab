<?php
/**
 * The template part for displaying content
 *
 * @package KniffTech
 * @subpackage Bidnis
 * @since Bidnis 1.0
 */

while( have_posts() ): the_post(); ?>
	<?php get_template_part('template-parts/content_header'); ?>
	
	<article id="post-<?php the_ID(); ?>" <?php post_class(); ?>>
			<?php the_content(); ?>

		<div style="clear: both; ">&nbsp;</div>
	</article>

	<?php if( get_theme_mod( 'entry_meta_tags', true ) && has_tag() ): ?>
		<div class="tags">
			<i class="fa fa-tag"></i>
			<?php the_tags('', ', ') ?>
		</div>
	<?php endif; ?>
	
	<?php if( is_single() ){
		wp_link_pages( array(
			'before'      => '<div class="pagination"><span class="page-links-title">' . __( 'Pages:', 'bidnis' ) . '</span>',
			'after'       => '</div>',
			'link_before' => '<span>',
			'link_after'  => '</span>',
			'pagelink'    => '<span class="screen-reader-text">' . __( 'Page', 'bidnis' ) . ' </span>%',
			'separator'   => '<span class="screen-reader-text">, </span>',
		) );
	} ?>

	<?php ( !is_page() ) ? get_template_part('template-parts/author_bio') : false; ?>

	<?php if( is_singular( 'attachment' ) ): ?>
		<div class="nav-links">
			<div class="nav-previous"><?php previous_image_link( false, __( 'Previous Image', 'bidnis' ) ); ?></div>
			<div class="nav-next"><?php next_image_link( false, __( 'Next Image', 'bidnis' ) ); ?></div>
		</div><!-- .nav-links -->
	
	<?php elseif ( is_singular( 'post' ) ):
		// Previous/next post navigation.
		the_post_navigation( array(
			'next_text' => '<span class="screen-reader-text">' . __( 'Next post:', 'bidnis' ) . '</span> ' .
				'<span class="post-title">%title</span>',
			'prev_text' => '<span class="meta-nav" aria-hidden="true"></span> ' .
				'<span class="screen-reader-text">' . __( 'Previous post:', 'bidnis' ) . '</span> ' .
				'<span class="post-title">%title</span>',
		) );
	endif; ?>
	
<?php endwhile; ?>