<?php

/*
 * @file
 * Contains some non-class-specific methods and things I'm not sure where to put.
 */

$values = array("/", "+");


/**
 * Initialize a pager for theme('pager') without running an SQL query.
 *
 * The nuts and bolts of this function are borrowed from z3950.module
 * and also apachesolr.module.
 *
 * @see pager_query()
 *
 * @param $total
 *  The total number of items found.
 * @param $limit
 *  The number of items you will display per page.
 * @param $element
 *  An optional integer to distinguish between multiple pagers on one page.
 *
 * @return
 *  The current page for $element. 0 by default if $_GET['page'] is empty.
 */
function islandora_solr_search_pager_init($total, $limit = 10, $element = 0) {
  global $pager_page_array, $pager_total, $pager_total_items;
  $page = isset($_GET['page']) ? $_GET['page'] : '';

  // Convert comma-separated $page to an array, used by other functions.
  $pager_page_array = explode(',', $page);

  // We calculate the total of pages as ceil(items / limit).
  $pager_total_items[$element] = $total;
  $pager_total[$element] = ceil($pager_total_items[$element] / $limit);
  $pager_page_array[$element] = max(0, min((int)$pager_page_array[$element], ((int)$pager_total[$element]) - 1));

  // return the current position
  return $pager_page_array[$element];
}

function restoreSlashes($str) {
  $replacements = array("~slsh~", "~plus~");
  $values = array("/", "+");
  return str_replace($replacements, $values, $str); //can't have a slash in the url breaks drupal clean urls
}

function replaceSlashes($str) {
  $replacements = array("~slsh~", "~plus~");
  $values = array("/", "+");
  return str_replace($values, $replacements, $str); // reverse of above
}


//a better explode method allows quotes in the returned strings.  taken from the php.net site
function csv_explode($delim=',', $str, $enclose='"', $preserve=FALSE) {
  $resArr = array();
  $n = 0;
  $expEncArr = explode($enclose, $str);
  foreach ($expEncArr as $EncItem) {
    if ($n++ %2) {
      array_push($resArr, array_pop($resArr) . ($preserve?$enclose:'') . $EncItem . ($preserve?$enclose:''));
    }
    else {
      $expDelArr = explode($delim, $EncItem);
      array_push($resArr, array_pop($resArr) . array_shift($expDelArr));
      $resArr = array_merge($resArr, $expDelArr);
    }
  }
  return $resArr;
}

//copied from apache solr service and added space as a value to escape.  We are using this to escape the field being searched not the text being searched for so we need both.
function lesser_escape($value) {
  //list taken from http://lucene.apache.org/java/docs/queryparsersyntax.html#Escaping%20Special%20Characters
  // Zac removed the quotation mark and whitespace from this list.
  // a) Spaces in our search strings don't need escaping
  // b) We want to allow users to use quotation marks to indicate multi-word phrases
 //  (alan) pulled the asterisk from excaping to allow wild cards
  $pattern = '/(\+|-|&&|\|\||!|\(|\)|\{|}|\[|]|\^|~|\?|:|\\\)/';
  $replace = '\\\$1';

  return preg_replace($pattern, $replace, $value);
}

//escape characters in field names of facets
function solr_escape($facets) {
  $returnFacets = array();
  foreach ($facets as $facet) {
    $tmp = drupal_substr($facet, 0, strpos($facet, ':"'));
    $tmp = lesser_escape(trim($tmp));

    $returnFacets[] = $tmp . drupal_substr($facet, strpos($facet, ':"'), drupal_strlen($facet));

  }
  return $returnFacets;
}


function islandora_build_substitution_list($rawFieldList) {
  $facetArray = array();
  $facetFields = '';
  $lines = preg_split('/[,|\n|\r]/', $rawFieldList);
  foreach ($lines as $line) {
    if( $line ) {
      $lineArray = explode('~', $line);
      $key = trim($lineArray[0]);
      $value = trim($lineArray[1]);
      if (!$value) {
        $value = $key;
      }
      $facetArray[$key] = $value;
      if ($facetFields) {
        $facetFields .= ",";
      }
      $facetFields .= $key;
    }
  }
  return $facetArray;
}

/**
   * checks to see if browser is IE and is less then the specified version
   * 
   * user agent can be spoofed but we are only using this to determine 
   * whether or not we should show the Date facet with the canvas graph
   * 
   * @param mixed $version_number
   *   version number ie 8
   * @return boolean 
   *   TRUE if browser is IE and version is less then $version_number
   *   otherwise FALSE
   */
  function islandora_solr_is_ie_less_then($version_number) {
    preg_match('/MSIE (.*?);/', $_SERVER['HTTP_USER_AGENT'], $matches);
    if (count($matches) > 1) {
      //Then we're using IE
      $version = $matches[1];
      if ($version < $version_number) {
        return TRUE;
      }
      else {
        return FALSE;
      }
    }
    //NOT ie
    return FALSE;
  }
