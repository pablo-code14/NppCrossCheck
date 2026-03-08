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

## Notes
* The output is appended to the end of the original document.
* Comparisons are case-insensitive.
* To make duplicate entries easier to identify, counts greater than one are displayed with leading zeros.
* The plugin can run on unsaved documents (e.g., "new 1").

## Installation

The plugin is currently pending official approval and will soon be available in the Notepad++ Plugin Admin.

### Manual Installation

1. Download the `NppCrossCheck_XYZ.zip` file from the latest release:  
   https://github.com/pablo-code14/NppCrossCheck/releases

   *Where `XYZ` indicates your system architecture: `x64`, `x86`, or `ARM64`.*

2. Close Notepad++ if it is running.

3. Create an `NppCrossCheck` folder in Notepad++'s plugins directory  
   (typically `C:\Program Files\Notepad++\Plugins`).

4. Extract the downloaded file into the new folder:  
   `.\Plugins\NppCrossCheck\NppCrossCheck.dll`

5. Start Notepad++ again and run the plugin.
   `Plugins -> NppCrossCheck` or press `ALT+L`