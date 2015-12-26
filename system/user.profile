<!---

    NOTE: This file holds all the data which will allow the use to modify the
    way the interface looks. NOT TO BE USED BY n00b5 because it can potentailly
    break horribly and potentially rupture the fabric of the universe.

    You have been warned.....

-->

<config>

    <keyBindings>

<!-- Tags are only key. ID is the identification name, code is the relevant SDL KeyCode -->
        <key id="up" code="119" />
        <key id="down" code="115" />
        <key id="left" code="97" />
        <key id="right" code="100" />

    </keyBindings>

    <fontList>
<!-- 
    Should list the fonts (name and the location) where name corresponds to
    the name that will be referred to throughout this file
 -->
        <default sauce="system/fonts/default.ttf" />
        <droidSans sauce="system/fonts/droidsansfallback.ttf" />
    </fontList>

    <options>
<!-- This should only affect the settings/options screen -->
        

    </options>

<!-- This is the configuration for the general interface (i.e. where everything fits/goes)
        Contains:
            -> Content (where the novelList, DLList and Settings will go)
            -> Menu (Where the menu will go) --> THIS IS OPTIONAL, since there is a menu section below which will specify all the parts of the menu, and if it's built right, you won't have spare space
 -->

    <interface>
        <content name="rendering" x="15" y="55" h="545" w="994" />
    </interface>

<!-- Note: Menu works in the following manner:
        - There is either image, button or content
        - They require a NAME (What the program will recognise it as), SAUCE(Location of the image), text (Contents of a button), X(x-pos of the object), Y (y-pos of the object), H (height of the object), W (Width of the object)
        - They are listed in the drawn order as well (so the first image will be drawn before the second image etc.)
    THESE IMAGES/BUTTON/CONTENT/FONT MUST EXIST OR THE CODE WILL BE CHANGED:
        - FONT: default
        - IMAGE: settings
        - IMAGE: settings-selected
        - button: novelList
        - button: reader (unused, since reader content is dead)
        - IMAGE: downloads
        - IMAGE: downloads-selected
        - IMAGE: downloads-inactive

There is the optional tag name COLOUR. This tag allows the modder to change the default colours (such as button colours, background colours and the screen clear colours). To change the button colour, just have the tag <colour> with the attributes name(which can be either back, clear or button), r, g, b, a (each of the rgba combinations must be a number between 0 - 255. Colour must exist before ANY buttons are (or else the colour won't appear)

There is only support for one content ATM

To leave the image at its original size (and only set the size), set height and width at -1
 -->
 
    <menu>
        <font name="default" />
        <image name="settings" sauce="system/images/settings.png" x="0" y="0" h="50" w="50"/>
        <image name="settings-selected" sauce="system/images/settings_invert.png" x="0" y="0" h="50" w="50" />
        <button name="novelList" text="Novel List" font="default" x="55" y="0" h="50" w="912" size="25"/>
        <!--button name="reader" text="Reader" font="default" x="513" y="0" h="50" w="453" size="25"/ -->
        <image name="downloads" sauce="system/images/download.png" x="974" y="0" h="50" w="50" />
        <image name="downloads-selected" sauce="system/images/download_invert.png" x="974" y="0" h="50" w="50" />
        <image name="downloads-inactive" sauce="system/images/update.png" x="974" y="0" h="50" w="50" />
    </menu>

    <novelList>
        <font value="default" />
        <size value="50" />
    </novelList>

</config>
