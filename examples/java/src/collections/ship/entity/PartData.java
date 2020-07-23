/*-
 * Copyright (c) 2002, 2020 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package collections.ship.entity;

import java.io.Serializable;

/**
 * A PartData serves as the value in the key/value pair for a part entity.
 *
 * <p> In this sample, PartData is used only as the storage data for the
 * value, while the Part object is used as the value's object representation.
 * Because it is used directly as storage data using serial format, it must be
 * Serializable. </p>
 *
 * @author Mark Hayes
 */
public class PartData implements Serializable {

    private final String name;
    private final String color;
    private final Weight weight;
    private final String city;

    public PartData(String name, String color, Weight weight, String city) {

        this.name = name;
        this.color = color;
        this.weight = weight;
        this.city = city;
    }

    public final String getName() {

        return name;
    }

    public final String getColor() {

        return color;
    }

    public final Weight getWeight() {

        return weight;
    }

    public final String getCity() {

        return city;
    }

    @Override
    public String toString() {

        return "[PartData: name=" + name +
	    " color=" + color +
	    " weight=" + weight +
	    " city=" + city + ']';
    }
}
