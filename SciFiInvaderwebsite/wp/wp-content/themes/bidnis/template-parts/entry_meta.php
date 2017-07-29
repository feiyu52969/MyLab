<?php
/**
 * The template part for displaying post meta data
 *
 * @package KniffTech
 * @subpackage Bidnis
 * @since Bidnis 1.0
 */
?>
<div class="entry-meta">
	<?php // author link
		if( get_theme_mod( 'entry_meta_author', true ) ){
			printf( '<span><i class="fa fa-user"></i> <a href="%1$s">%2$s</a></span>',
				esc_url( get_author_posts_url( get_the_author_meta( 'ID' ) ) ),
				get_the_author()
				);
		}
	?>
	
	<?php // posted date
		if( get_theme_mod( 'entry_meta_date', true ) ){
			printf( '<span><i class="fa fa-calendar"></i> <a href="%1$s">%2$s</a></span>',
				esc_url( get_permalink() ),
				get_the_date()
			);
		}
	?>

	<?php // attachment image dimentions
		if( wp_attachment_is_image() ){
			$metadata = wp_get_attachment_metadata();

			printf( '<span><i class="fa fa-camera"></i> %1$s x %2$s</span>',
				absint( $metadata['width'] ),
				absint( $metadata['height'] )
			);
		}
	?>

	<?php // comments
		$bidnis_comments_count = get_comments_number();
		if( get_theme_mod( 'entry_meta_comments', true ) && $bidnis_comments_count > 0 && comments_open() ){
			
			printf( '<span><i class="fa fa-comment-o"></i> <a href="%1$s/#comments">%2$s</a></span>',
				esc_url( get_permalink() ), 
				$bidnis_comments_count
			);
		}
	?>

	<?php	// post format
		$bidnis_format = get_post_format();
		
		if ( get_theme_mod( 'entry_meta_post_format', true ) && current_theme_supports( 'post-formats', $bidnis_format ) ){
			switch($bidnis_format){
				case 'gallery':
				case 'image':
					$bidnis_class = 'fa-image';
					break;

				case 'video':
					$bidnis_class = 'fa-file-video-o';
					break;

				case 'quote':
					$bidnis_class = 'fa-quote-right';
					break;

				case 'link':
					$bidnis_class = 'fa-link';
					break;

				case 'status':
					$bidnis_class = 'fa-exclamation';
					break;

				case 'audio':
					$bidnis_class = 'fa-music';
					break;

				case 'chat':
					$bidnis_class = 'fa-comments';
					break;

				default:
					$bidnis_class = 'fa-file-o';
					break;
			}

			printf( '<span class="entry-format"><i class="fa ' . $bidnis_class . '"></i> %1$s<a href="%2$s">%3$s</a></span>',
				sprintf( '<span class="screen-reader-text">%s </span>', _x( 'Format', 'Used before post format.', 'bidnis' ) ),
				esc_url( get_post_format_link( $bidnis_format ) ),
				get_post_format_string( $bidnis_format )
			);
	} ?>
	
	<?php // categories
		if( get_theme_mod( 'entry_meta_categories', true ) && has_category() ): ?>
			<span>
				<i class="fa fa-folder-o"></i>
				<?php the_category(', '); ?>
			</span>
		<?php endif; 
	?>

	<?php // edit link
		if( is_user_logged_in() ): ?>
			<span>
				<i class="fa fa-pencil"></i>
				<?php edit_post_link( __('Edit', 'bidnis') ); ?>
			</span>
		<?php endif; 
	?>

</div>