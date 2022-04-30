# Format for the <config_name>.conf file
- As you can read in the README.md, the config file must be placed in $HOME/.config/gstuff.

- It should be a text file (ASCII or UTF-8) with one line for each attribute, formatted like this:<br>
&emsp;&emsp; **attribute:**   *(..as many spaces as you want..)*  **value**

- You can add empty lines between attributes and at the start/end of the config file.

- The folowing table illustrates each possible attribute and its possible values

| ATTRIBUTE      | DESCRIPTION                                                                                               | DEFAULT VALUE |
|----------------|-----------------------------------------------------------------------------------------------------------|---------------|
| background     | Hexadecimal RGB value                                                                                     | **#323200**   |
| borderColor    | Hexadecimal RGB value                                                                                     | **#FFFF44**   |
| textColor      | Hexadecimal RGB value                                                                                     | **#FFFFFF**   |
| duration       | Duration of the banner **in ms**                                                                          | **2000**      |
| padding        | Padding from banner to the border of the screen **in percentage**                                         | **4**         |
| border         | Size of the solid-color border around the banner **in pixel**                                             | **5**         |
| position       | Position of the banner on the screen, can be **TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT, CENTER**   | **CENTER**    |
| paddingInside  | Padding from the border of the banner to the text **in pixel**                                            | **30**        |
| interlineSpace | Space between two lines **in pixel**                                                                      | **2**         |
| textFormat     | Text formatting, can be **LEFT, RIGHT, CENTER**                                                           | **CENTER**    |
| display        | Which display to show the banner to, can be PRIMARY or a sequence of positive integers separated by comma | **PRIMARY**   |
| fontSize       | Size of the font                                                                                          | **30**        |
| fontName       | Name of the font, you can get a list with the "xlsfonts" command                                          | **fixed**     |
