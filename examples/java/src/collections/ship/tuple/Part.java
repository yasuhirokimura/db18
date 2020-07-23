/*-
 * Copyright (c) 2002, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package collections.ship.tuple;

/**
 * A Part represents the combined key/data pair for a part entity.
 *
 * <p> In this sample, Part is created from the stored key/data entry using a
 * SerialSerialBinding.  See {@link SampleViews.PartBinding} for details.
 * Since this class is not directly used for data storage, it does not need to
 * be Serializable. </p>
 *
 * @author Mark Hayes
 */
public class Part {

    private final String number;
    private final String name;
    private final String color;
    private final Weight weight;
    private final String city;

    public Part(String number, String name, String color, Weight weight,
                String city) {

        this.number = number;
        this.name = name;
        this.color = color;
        this.weight = weight;
        this.city = city;
    }

    public final String getNumber() {

        return number;
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

        return "[Part: number=" + number +
               " name=" + name +
               " color=" + color +
               " weight=" + weight +
               " city=" + city + ']';
    }
}
