<?php
/**
 * Custom functions that act independently of the theme templates.
 *
 * Eventually, some of the functionality here could be replaced by core features.
 *
 * @package Elegant_Pink
 */

/**
* Adds custom classes to the array of body classes.
*
* @param array $classes Classes for the body element.
* @return array
*/
function elegant_pink_body_classes( $classes ) {
	global $post;
    // Adds a class of group-blog to blogs with more than 1 published author.
	if ( is_multi_author() ) {
		$classes[] = 'group-blog';
	}
    
    // Adds a class of custom-background-image to sites with a custom background image.
	if ( get_background_image() ) {
		$classes[] = 'custom-background-image';
	}
    
    // Adds a class of custom-background-color to sites with a custom background color.
    if ( get_background_color() != 'ffffff' ) {
		$classes[] = 'custom-background-color';
	}
    
	// Adds a class of hfeed to non-singular pages.
	if ( ! is_singular() ) {
		$classes[] = 'hfeed';
	}
    
    if( ( is_front_page() && is_home() ) || is_home() ){
        $home_layout = get_theme_mod( 'elegant_pink_home_page_layout', 'sidebar' );        
        if( $home_layout == 'fullwidth' )    
        $classes[] = 'full-width';
    }
    
    if( is_page() ){
        $sidebar_layout = get_post_meta( $post->ID, 'elegant_pink_sidebar_layout', true );
        if( $sidebar_layout == 'no-sidebar' )
		$classes[] = 'full-width';
    }
    
    if( ! is_active_sidebar( 'right-sidebar' ) ) {
		$classes[] = 'full-width';	
	}
    
	return $classes;
}
add_filter( 'body_class', 'elegant_pink_body_classes' );

/**
* Adds custom classes to the array of post classes.
*
* @param array $classes Classes for the post element.
* @return array
*/
function elegant_pink_post_classes( $classes ) {
    $classes[] = 'latest_post';
    
    return $classes;
}
add_filter( 'post_class', 'elegant_pink_post_classes' );

/**
 * Callback for Social Links 
 */
 function elegant_pink_social_cb(){
    $facebook    = get_theme_mod( 'elegant_pink_facebook' );
    $twitter     = get_theme_mod( 'elegant_pink_twitter' );
    $google_plus = get_theme_mod( 'elegant_pink_google_plus' );
    $linkedin    = get_theme_mod( 'elegant_pink_linkedin' );
    $youtube     = get_theme_mod( 'elegant_pink_youtube' );
    $instagram   = get_theme_mod( 'elegant_pink_instagram' );
    $pinterest   = get_theme_mod( 'elegant_pink_pinterest' );
    
    if( $facebook || $twitter || $google_plus || $linkedin || $youtube || $instagram || $pinterest ){
    ?>
    <ul class="social-networks">
		<?php if( $facebook ){?>
            <li><a href="<?php echo esc_url( $facebook );?>" target="_blank" title="<?php esc_html_e( 'Facebook', 'elegant-pink' ); ?>"><span class="fa fa-facebook"></span></a></li>
		<?php } if( $twitter ){?>    
            <li><a href="<?php echo esc_url( $twitter );?>" target="_blank" title="<?php esc_html_e( 'Twitter', 'elegant-pink' ); ?>"><span class="fa fa-twitter"></span></a></li>
		<?php } if( $google_plus ){?>
            <li><a href="<?php echo esc_url( $google_plus );?>" target="_blank" title="<?php esc_html_e( 'Google Plus', 'elegant-pink' ); ?>"><span class="fa fa-google-plus"></span></a></li>
		<?php } if( $linkedin ){?>
            <li><a href="<?php echo esc_url( $linkedin );?>" target="_blank" title="<?php esc_html_e( 'LinkedIn', 'elegant-pink' ); ?>"><span class="fa fa-linkedin"></span></a></li>
		<?php } if( $youtube ){?>
            <li><a href="<?php echo esc_url( $youtube );?>" target="_blank" title="<?php esc_html_e( 'YouTube', 'elegant-pink' ); ?>"><span class="fa fa-youtube"></span></a></li>
		<?php } if( $instagram ){?>
            <li><a href="<?php echo esc_url( $instagram );?>" target="_blank" title="<?php esc_html_e( 'Instagram', 'elegant-pink' ); ?>"><span class="fa fa-instagram"></span></a></li>
        <?php } if( $pinterest ){?>
            <li><a href="<?php echo esc_url( $pinterest );?>" target="_blank" title="<?php esc_html_e( 'Pinterest', 'elegant-pink' ); ?>"><span class="fa fa-pinterest-p"></span></a></li>            
        <?php }?>
	</ul>
    <?php
    }
}
add_action( 'elegant_pink_social', 'elegant_pink_social_cb' );
 
if( ! function_exists( 'elegant_pink_excerpt' ) ):  
/**
 * elegant_pink_excerpt can truncate a string up to a number of characters while preserving whole words and HTML tags
 *
 * @param string $text String to truncate.
 * @param integer $length Length of returned string, including ellipsis.
 * @param string $ending Ending to be appended to the trimmed string.
 * @param boolean $exact If false, $text will not be cut mid-word
 * @param boolean $considerHtml If true, HTML tags would be handled correctly
 *
 * @return string Trimmed string.
 * 
 * @link http://alanwhipple.com/2011/05/25/php-truncate-string-preserving-html-tags-words/
 */
function elegant_pink_excerpt($text, $length = 100, $ending = '...', $exact = false, $considerHtml = true) {
	if ($considerHtml) {
		// if the plain text is shorter than the maximum length, return the whole text
		if (strlen(preg_replace('/<.*?>/', '', $text)) <= $length) {
			return $text;
		}
		// splits all html-tags to scanable lines
		preg_match_all('/(<.+?>)?([^<>]*)/s', $text, $lines, PREG_SET_ORDER);
		$total_length = strlen($ending);
		$open_tags = array();
		$truncate = '';
		foreach ($lines as $line_matchings) {
			// if there is any html-tag in this line, handle it and add it (uncounted) to the output
			if (!empty($line_matchings[1])) {
				// if it's an "empty element" with or without xhtml-conform closing slash
				if (preg_match('/^<(\s*.+?\/\s*|\s*(img|br|input|hr|area|base|basefont|col|frame|isindex|link|meta|param)(\s.+?)?)>$/is', $line_matchings[1])) {
					// do nothing
				// if tag is a closing tag
				} else if (preg_match('/^<\s*\/([^\s]+?)\s*>$/s', $line_matchings[1], $tag_matchings)) {
					// delete tag from $open_tags list
					$pos = array_search($tag_matchings[1], $open_tags);
					if ($pos !== false) {
					unset($open_tags[$pos]);
					}
				// if tag is an opening tag
				} else if (preg_match('/^<\s*([^\s>!]+).*?>$/s', $line_matchings[1], $tag_matchings)) {
					// add tag to the beginning of $open_tags list
					array_unshift($open_tags, strtolower($tag_matchings[1]));
				}
				// add html-tag to $truncate'd text
				$truncate .= $line_matchings[1];
			}
			// calculate the length of the plain text part of the line; handle entities as one character
			$content_length = strlen(preg_replace('/&[0-9a-z]{2,8};|&#[0-9]{1,7};|[0-9a-f]{1,6};/i', ' ', $line_matchings[2]));
			if ($total_length+$content_length> $length) {
				// the number of characters which are left
				$left = $length - $total_length;
				$entities_length = 0;
				// search for html entities
				if (preg_match_all('/&[0-9a-z]{2,8};|&#[0-9]{1,7};|[0-9a-f]{1,6};/i', $line_matchings[2], $entities, PREG_OFFSET_CAPTURE)) {
					// calculate the real length of all entities in the legal range
					foreach ($entities[0] as $entity) {
						if ($entity[1]+1-$entities_length <= $left) {
							$left--;
							$entities_length += strlen($entity[0]);
						} else {
							// no more characters left
							break;
						}
					}
				}
				$truncate .= substr($line_matchings[2], 0, $left+$entities_length);
				// maximum lenght is reached, so get off the loop
				break;
			} else {
				$truncate .= $line_matchings[2];
				$total_length += $content_length;
			}
			// if the maximum length is reached, get off the loop
			if($total_length>= $length) {
				break;
			}
		}
	} else {
		if (strlen($text) <= $length) {
			return $text;
		} else {
			$truncate = substr($text, 0, $length - strlen($ending));
		}
	}
	// if the words shouldn't be cut in the middle...
	if (!$exact) {
		// ...search the last occurance of a space...
		$spacepos = strrpos($truncate, ' ');
		if (isset($spacepos)) {
			// ...and cut the text in this position
			$truncate = substr($truncate, 0, $spacepos);
		}
	}
	// add the defined ending to the text
	$truncate .= $ending;
	if($considerHtml) {
		// close all unclosed html-tags
		foreach ($open_tags as $tag) {
			$truncate .= '</' . $tag . '>';
		}
	}
	return $truncate;
}
endif; // End function_exists

/**
 * Callback function for Home Page Slider 
 */
function elegant_pink_slider_cb(){
    $slider_caption  = get_theme_mod( 'elegant_pink_slider_caption', '1' );
    $slider_readmore = get_theme_mod( 'elegant_pink_slider_readmore', __( 'Read More', 'elegant-pink' ) );
    $slider_type     = get_theme_mod( 'elegant_pink_slider_type', 'category' );
    $slider_cat      = get_theme_mod( 'elegant_pink_slider_cat' );
    $slider_post1    = get_theme_mod( 'elegant_pink_slider_post1' );
    $slider_post2    = get_theme_mod( 'elegant_pink_slider_post2' );
    $slider_post3    = get_theme_mod( 'elegant_pink_slider_post3' );
    $slider_char     = get_theme_mod( 'elegant_pink_slider_char', 100 );
    
    $sliders = array( $slider_post1, $slider_post2, $slider_post3 );	    
	$sliders = array_diff( array_unique( $sliders ), array('') );
    
    if( ( $slider_type == 'category' ) && $slider_cat ){
        $qry = new WP_Query( array(
            'post_type'     => 'post',
            'post_status'   => 'publish',
            'posts_per_page'=> -1,
            'cat'           => $slider_cat,
        ));
        if( $qry->have_posts() ){ ?>
        <div class="slideshow">
            <ul id="imageGallery">
            <?php
            while( $qry->have_posts() ){
                $qry->the_post();
                $image = wp_get_attachment_image_src( get_post_thumbnail_id(), 'elegant-pink-slider' );
                
                ?>
                <li>
                    <a href="<?php the_permalink(); ?>"><img src="<?php echo esc_url( $image[0] ); ?>" alt="<?php the_title(); ?>" /></a>
    				<?php if( $slider_caption ){ ?>
                    <div class="banner-text">
    					<div class="text">
    						<?php elegant_pink_category(); ?>    						
                            <strong class="title"><?php the_title(); ?></strong>
    						<?php 
                            if( has_excerpt() ){
                                the_excerpt();  
    						}else{ ?>
                                <p><?php echo elegant_pink_truncate( get_the_content(), $slider_char ); ?></p>
                            <?php
                            }
    						?>
                            <a href="<?php the_permalink();?>" class="btn-readmore"><?php echo esc_attr( $slider_readmore ); ?></a>
    					</div>
    				</div>
                    <?php } ?>
                </li>
                <?php                
            }
            wp_reset_postdata();
            ?>
            </ul>
        </div>
        <?php
        }
    }
    
    if( ( $slider_type == 'post' ) && $sliders ){
        $qry = new WP_Query( array(
            'post_type'   => 'post',
            'post_status' => 'publish',
            'post__in'    => $sliders,
            'orderby'     => 'post__in'
        ));
        if( $qry->have_posts() ){ ?>
        <div class="slideshow">
            <ul id="imageGallery">
            <?php
            while( $qry->have_posts() ){
                $qry->the_post();
                $image = wp_get_attachment_image_src( get_post_thumbnail_id(), 'elegant-pink-slider' );
                
                ?>
                <li>
                    <img src="<?php echo esc_url( $image[0] ); ?>" alt="<?php the_title(); ?>" />
    				<?php if( $slider_caption ){ ?>
                    <div class="banner-text">
    					<div class="text">
    						<?php elegant_pink_category(); ?>
    						<strong class="title"><?php the_title(); ?></strong>
    						<?php 
                            if( has_excerpt() ){
                                the_excerpt();  
    						}else{ ?>
                                <p><?php echo elegant_pink_truncate( get_the_content(), $slider_char ); ?></p>
                            <?php
                            }
    						?>
    						<a href="<?php the_permalink();?>" class="btn-readmore"><?php echo esc_attr( $slider_readmore ); ?></a>
    					</div>
    				</div>
                    <?php }?>
                </li>
                <?php                
            }
            wp_reset_postdata(); 
            ?>
            </ul>
        </div>
        <?php
        }        
    }
}
add_action( 'elegant_pink_slider', 'elegant_pink_slider_cb' );

/**
 * Hook to move comment text field to the bottom in WP 4.4
 * 
 * @link http://www.wpbeginner.com/wp-tutorials/how-to-move-comment-text-field-to-bottom-in-wordpress-4-4/  
 */
function elegant_pink_move_comment_field_to_bottom( $fields ) {
    $comment_field = $fields['comment'];
    unset( $fields['comment'] );
    $fields['comment'] = $comment_field;
    return $fields;
}
add_filter( 'comment_form_fields', 'elegant_pink_move_comment_field_to_bottom' );

/**
 * Callback function for Comment List
 * 
 * @link https://codex.wordpress.org/Function_Reference/wp_list_comments 
 */
function elegant_pink_theme_comment( $comment, $args, $depth ){
    $GLOBALS['comment'] = $comment;
	extract($args, EXTR_SKIP);

	if ( 'div' == $args['style'] ) {
		$tag = 'div';
		$add_below = 'comment';
	} else {
		$tag = 'li';
		$add_below = 'div-comment';
	}
?>
	<<?php echo $tag ?> <?php comment_class( empty( $args['has_children'] ) ? '' : 'parent' ) ?> id="comment-<?php comment_ID() ?>">
	<?php if ( 'div' != $args['style'] ) : ?>
	<div id="div-comment-<?php comment_ID() ?>" class="comment-body">
	<?php endif; ?>
	<div class="comment-author vcard">
	<?php if ( $args['avatar_size'] != 0 ) echo get_avatar( $comment, $args['avatar_size'] ); ?>
	<?php printf( __( '<b class="fn">%s</b> <span class="says">says:</span>', 'elegant-pink' ), get_comment_author_link() ); ?>
	</div>
	<?php if ( $comment->comment_approved == '0' ) : ?>
		<em class="comment-awaiting-moderation"><?php _e( 'Your comment is awaiting moderation.', 'elegant-pink' ); ?></em>
		<br />
	<?php endif; ?>

	<div class="comment-meta commentmetadata">
    <a href="<?php echo htmlspecialchars( get_comment_link( $comment->comment_ID ) ); ?>">
		<time>
        <?php
			/* translators: 1: date, 2: time */
			printf( __( '%1$s', 'elegant-pink' ), get_comment_date() ); ?>
        </time>
    </a>
	</div>
    
    <div class="comment-content"><?php comment_text(); ?></div>
    
	<div class="reply">
	<?php comment_reply_link( array_merge( $args, array( 'add_below' => $add_below, 'depth' => $depth, 'max_depth' => $args['max_depth'] ) ) ); ?>
	</div>
	<?php if ( 'div' != $args['style'] ) : ?>
	</div>
	<?php endif; ?>
<?php
}

/**
 * Custom Pagination
*/
function elegant_pink_pagination(){
    $pagination = get_theme_mod( 'elegant_pink_pagination_type', 'default' );
    
    switch( $pagination ){
        case 'default': // Default Pagination
        
        the_posts_navigation();
        
        break;
        
        case 'load_more': // Load More Button
        
        echo '<div class="pagination"></div>';
        
        break;
        
        default:
        
        the_posts_navigation();
        
        break;
    }   
}

/**
 * Custom CSS
*/
function elegant_pink_custom_css(){
    $custom_css = get_theme_mod( 'elegant_pink_custom_css' );
    if( !empty( $custom_css ) ){
		echo '<style type="text/css">';
		echo wp_strip_all_tags( $custom_css );
		echo '</style>';
	}
}
add_action( 'wp_head', 'elegant_pink_custom_css', 100 );

if ( ! function_exists( 'elegant_pink_excerpt_more' ) && ! is_admin() ) :
/**
 * Replaces "[...]" (appended to automatically generated excerpts) with ... * 
 */
function elegant_pink_excerpt_more() {
	return ' &hellip; ';
}
add_filter( 'excerpt_more', 'elegant_pink_excerpt_more' );
endif;

if ( ! function_exists( 'elegant_pink_excerpt_length' ) ) :
/**
 * Changes the default 55 character in excerpt 
*/
function elegant_pink_excerpt_length( $length ) {
	return 15;
}
add_filter( 'excerpt_length', 'elegant_pink_excerpt_length', 999 );
endif;

/**
 * Footer Credits 
*/
function elegant_pink_footer_credit(){
    
    $text  = '<div class="site-info"><span>';
    $text .=  esc_html__( 'Copyright &copy; ', 'elegant-pink' ) . date('Y'); 
    $text .= ' <a href="' . esc_url( home_url( '/' ) ) . '">' . esc_html( get_bloginfo( 'name' ) ) . '</a>.</span>';
    $text .= '<span>';
    $text .= '<a href="' . esc_url( 'http://raratheme.com/wordpress-themes/elegant-pink/' ) .'" rel="author" target="_blank">' . esc_html__( 'Elegant Pink by: Rara Theme', 'elegant-pink' ) .'</a>';
    $text .= '</span><span>';
    $text .= sprintf( esc_html__( 'Powered by: %s', 'elegant-pink' ), '<a href="'. esc_url( __( 'https://wordpress.org/', 'elegant-pink' ) ) .'" target="_blank">WordPress</a>' );
    $text .= '</span></div>';
    echo apply_filters( 'elegant_pink_footer_text', $text );    
}
add_action( 'elegant_pink_footer', 'elegant_pink_footer_credit' );

/**
 * Return sidebar layouts for pages
*/
function elegant_pink_sidebar_layout(){
    global $post;
    
    if( get_post_meta( $post->ID, 'elegant_pink_sidebar_layout', true ) ){
        return get_post_meta( $post->ID, 'elegant_pink_sidebar_layout', true );    
    }else{
        return 'right-sidebar';
    }
}

/**
 * Query Jetpack activation
*/
function is_jetpack_activated( $gallery = false ){
	if( $gallery ){
        return ( class_exists( 'jetpack' ) && Jetpack::is_module_active( 'tiled-gallery' ) ) ? true : false;
	}else{
        return class_exists( 'jetpack' ) ? true : false;
    }           
}

/**
 * Return Striptags from the content.
*/
function elegant_pink_truncate( $content, $letter_count ){
	$striped_content = strip_shortcodes( $content );
	$striped_content = strip_tags( $striped_content );
	$excerpt         = mb_substr( $striped_content, 0, $letter_count );
	
    if( $striped_content > $excerpt ){
		$excerpt .= '...';
	}
	
    return $excerpt;
}