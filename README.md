# NppCrossCheck plugin for Notepad++  
Compares two lists in the current document.  

## Usage
1. Place the two lists separated by one or more blank lines.
2. Run the plugin to cross-check their contents:
   `Plugins -> NppCrossCheck` or press `ALT+L`

## Example
Comparison of two lists of cities:  

### Input
```
Berlin
New York
London
Tokyo
Buenos Aires
Seville
Paris
Toronto
São Paulo
Mexico City
Buenos Aires
Rome
Shanghai
Singapore
Istanbul
Mexico City
Moscow

Singapore
Bangkok
Istanbul
Nairobi
Toronto
Moscow
Mexico city
Los Angeles
Rome
Toronto
Shanghai
Hong Kong
Seville
Sydney
Hong Kong
```

### Output
```
=====================
     COMMON (8)
=====================
 L1  L2
---------
  1   1   Istanbul
 02   1   Mexico City
  1   1   Moscow
  1   1   Rome
  1   1   Seville
  1   1   Shanghai
  1   1   Singapore
  1  02   Toronto


=====================
   LIST 1 ONLY (7)
=====================
  #
-----
  1   Berlin
 02   Buenos Aires
  1   London
  1   New York
  1   Paris
  1   São Paulo
  1   Tokyo


=====================
   LIST 2 ONLY (5)
=====================
  #
-----
  1   Bangkok
 02   Hong Kong
  1   Los Angeles
  1   Nairobi
  1   Sydney
```

## Using Comments

A new feature has been added that allows users to add comments to multiple lines. These comments are ignored during the comparison process.

By default, use `//` to add comments. The comment delimiter can be configured by going to:

**Plugins → NppCrossCheck → Edit Configuration File**

See the following example:

```text
Belgium
Spain       // this is a comment
Austria

Spain
Canada
United States


=====================
     COMMON (1)
=====================
 L1  L2
---------
  1   1   Spain // this is a comment

=====================
   LIST 1 ONLY (2)
=====================
  #
-----
  1   Austria
  1   Belgium

=====================
   LIST 2 ONLY (2)
=====================
  #
-----
  1   Canada
  1   United States
```

## Notes
* The output is appended to the end of the original document.
* Comparisons are case-insensitive.
* To make duplicate entries easier to identify, counts greater than one are displayed with leading zeros.
* The plugin can run on unsaved documents (e.g., "new 1").

## Installation

Available in the _Plugin Admin_ from the Notepad++ version v8.9.3 (Released on the 24th of March, 2026)

Instructions (English):
1. In the Menu Bar, go to _Plugins_ → _Plugins Admin..._
2. Search and select "NppCrossCheck"
3. Install

Intrucciones (Español):
1. En la barra de menús, ve a _Complementos_ → _Administrar Complementos..._
2. Busca y selecciona "NppCrossCheck"
3. Instalar
