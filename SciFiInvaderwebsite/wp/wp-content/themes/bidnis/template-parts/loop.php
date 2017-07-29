<?php
/**
 * Content loop
 *
 * @package KniffTech
 * @subpackage Bidnis
 * @since Bidnis 1.0
 */
if( have_posts() ):
	while( have_posts() ): the_post(); ?>
		<article <?php post_class(); ?>>
			<header class="archive-entry-header">
				<?php // Title
					if ( is_sticky() && is_home() && ! is_paged() ){
						the_title('<h3><a href="' . esc_url( get_the_permalink() ) . '">','</a></h3>');
					}else if( get_the_title() != '' ){
						the_title('<h3></i><a href="' . esc_url( get_the_permalink() ) . '">','</a></h3>');
				
					}else{
						printf('<h3><a href="%1$s">%2$s</a></h3>',
							esc_url( get_the_permalink() ),
							__('Untitled post', 'bidnis')
						);
					}
				?>

				<?php get_template_part('template-parts/entry_meta'); ?>
			</header>

			<?php // Post thumbnail
			if( has_post_thumbnail() && get_theme_mod('thumbnail_index', true) ): ?>
				<a class="post-thumbnail" href="<?php the_permalink(); ?>" aria-hidden="true">
					<?php the_post_thumbnail( 'post-thumbnail', array( 'alt' => the_title_attribute( 'echo=0' ) ) ); ?>
				</a>
			<?php endif; ?>
		
			<div class="archive-endtry-content">
				<?php // Excerpt or full content
					if( get_theme_mod( 'display_content', false ) ){
						the_content( sprintf(
							__( 'Continue reading<span class="screen-reader-text"> "%s"</span>', 'bidnis' ),
							get_the_title()
						) );
					
					}else{
						the_excerpt();
						
						// Read more 
						if( get_theme_mod('read_more', true) ): ?>
							<a class="read-more" title="<?php esc_attr( the_title() ); ?>" href="<?php the_permalink(); ?>">
								<?php _e( 'Read more', 'bidnis' ); ?>
							</a>
						<?php endif;
					}
				?>
				
				<div style="clear: both; height: 0px;">&nbsp;</div>
			</div>
		</article>
	<?php endwhile; ?>

	<?php
		if( get_theme_mod('page_numbers', false) ){
			the_posts_pagination( array(
				'prev_text'          => __( 'Previous page', 'bidnis' ),
				'next_text'          => __( 'Next page', 'bidnis' ),
				'before_page_number' => '<span class="meta-nav screen-reader-text">' . __( 'Page', 'bidnis' ) . ' </span>',
			) );

		}else{
			the_posts_navigation( array(
				'prev_text' => __( 'Back', 'bidnis' ),
				'next_text' => __( 'Next', 'bidnis' ),
			) ); 
		}
	?>

<?php else: ?>
	
	<?php get_template_part( 'template-parts/content', 'none' ); ?>

<?php endif; ?>