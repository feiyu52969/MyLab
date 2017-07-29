<?php
/**
 * Template for displaying images
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
    <?php while ( have_posts() ) : the_post(); ?>
      <?php get_template_part('template-parts/content_header'); ?>

      <article id="post-<?php the_ID(); ?>" <?php post_class(); ?>>
        <div class="entry-attachment">
            <?php echo wp_get_attachment_image( get_the_ID(), 'large' ); ?>
            <?php the_excerpt(); ?>
        </div>
      </article>

      <nav id="image-navigation" class="navigation image-navigation">
        <div class="nav-links">
          <div class="nav-previous"><?php previous_image_link( false, __( 'Previous Image', 'bidnis' ) ); ?></div>
          <div class="nav-next"><?php next_image_link( false, __( 'Next Image', 'bidnis' ) ); ?></div>
        </div><!-- .nav-links -->
      </nav><!-- .image-navigation -->
      
      <?php if ( comments_open() || get_comments_number() ) comments_template(); ?>
    
    <?php endwhile; ?>
  </main>

	<?php get_sidebar(); ?>
</div>

<?php get_footer(); ?>